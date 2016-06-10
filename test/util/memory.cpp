#ifdef __linux__

#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <utility>

#include <unistd.h>
#include <malloc.h>

using namespace mbgl;
using namespace std::literals::string_literals;

long getRSS() {
    auto statm = util::read_file("/proc/self/statm");

    std::vector<std::string> stats;
    std::istringstream stream(statm);

    std::copy(std::istream_iterator<std::string>(stream),
        std::istream_iterator<std::string>(),
        std::back_inserter(stats));

    return std::stol(stats[1]) * getpagesize();
}

class MemoryTest {
public:
    MemoryTest() {
        fileSource.styleResponse = [&](const Resource& res) { return response("style_" + getType(res) + ".json");};
        fileSource.tileResponse = [&](const Resource& res) { return response(getType(res) + ".tile"); };
        fileSource.sourceResponse = [&](const Resource& res) { return response("source_" + getType(res) + ".json"); };
        fileSource.glyphsResponse = [&](const Resource&) { return response("glyphs.pbf"); };
        fileSource.spriteJSONResponse = [&](const Resource&) { return response("sprite.json"); };
        fileSource.spriteImageResponse = [&](const Resource&) { return response("sprite.png"); };
    }

    util::RunLoop runLoop;
    std::shared_ptr<HeadlessDisplay> display { std::make_shared<mbgl::HeadlessDisplay>() };
    HeadlessView view { display, 1 };
    StubFileSource fileSource;

private:
    Response response(const std::string& path) {
        Response result;

        auto it = cache.find(path);
        if (it != cache.end()) {
            result.data = it->second;
        } else {
            auto data = std::make_shared<std::string>(
                util::read_file("test/fixtures/resources/"s + path));

            cache.insert(it, std::make_pair(path, data));
            result.data = data;
        }

        return result;
    }

    std::string getType(const Resource& res) {
        if (res.url.find("satellite") != std::string::npos) {
            return "raster";
        } else {
            return "vector";
        }
    };

    std::unordered_map<std::string, std::shared_ptr<std::string>> cache;
};

TEST(Memory, Vector) {
    MemoryTest test;

    Map map(test.view, test.fileSource, MapMode::Still);
    map.setZoom(16); // more map features
    map.setStyleURL("mapbox://streets");

    test::render(map);
    test.runLoop.runOnce();
}

TEST(Memory, Raster) {
    MemoryTest test;

    Map map(test.view, test.fileSource, MapMode::Still);
    map.setStyleURL("mapbox://satellite");

    test::render(map);
    test.runLoop.runOnce();
}

TEST(Memory, TEST_REQUIRES_LINUX_RELEASE(Load)) {
    MemoryTest test;

    Map map(test.view, test.fileSource, MapMode::Still);
    map.setZoom(16);

    auto renderRaster = [&] {
        map.setStyleURL("mapbox://satellite");
        test::render(map);
    };

    auto renderVector = [&] {
        map.setStyleURL("mapbox://streets");
        test::render(map);
    };

    const unsigned runs = 50;

    long totalRSS1 = 0;
    long totalRSS2 = 0;

    // Warm up buffers and cache.
    for (unsigned i = 0; i < 10; ++i) {
        renderRaster();
        renderVector();
    }

    for (unsigned i = 0; i < runs; ++i) {
        renderRaster();
        renderVector();
        totalRSS1 += getRSS();
    }

    for (unsigned i = 0; i < runs; ++i) {
        renderRaster();
        renderVector();
        totalRSS2 += getRSS();
    }

    // Compare the memory growth between the
    // average of two runs.
    long deltaRSS = (totalRSS2 - totalRSS1) / runs;

    ASSERT_LT(deltaRSS, 5 * 1024 * 1024) << "\
        Abnormal memory growth detected.";
}

TEST(Memory, TEST_REQUIRES_LINUX_RELEASE(Fragmentation)) {
    MemoryTest test;

    // Warm up buffers and cache.
    for (unsigned i = 0; i < 10; ++i) {
        Map map(test.view, test.fileSource, MapMode::Still);
        map.setZoom(16);

        map.setStyleURL("mapbox://satellite");
        test::render(map);

        map.setStyleURL("mapbox://streets");
        test::render(map);
    };

    // Process close callbacks, mostly needed by
    // libuv runloop.
    test.runLoop.runOnce();

    std::vector<std::unique_ptr<Map>> maps;
    long initialRSS = getRSS();
    unsigned runs = 20;

    for (unsigned i = 0; i < runs; ++i) {
        auto map = std::make_unique<Map>(test.view, test.fileSource, MapMode::Still);
        map->setZoom(16);

        map->setStyleURL("mapbox://satellite");
        test::render(*map);

        map->setStyleURL("mapbox://streets");
        test::render(*map);

        maps.push_back(std::move(map));
    };

    long mapObjMemoryFootprint = (getRSS() - initialRSS) / runs;

    ASSERT_LT(mapObjMemoryFootprint, 30 * 1024 * 1024) << "\
        Lets keep the mbgl::Map footprint lower than 30MB.";

    maps.clear();
    test.runLoop.runOnce();

    // Hint malloc to reclaim memory, otherwise it is way
    // too conservative.
    malloc_trim(0);

    long mapObjReclaimedMemory = mapObjMemoryFootprint - (getRSS() - initialRSS) / runs;

    // This is way too low, should be more.
    ASSERT_GT(float(mapObjReclaimedMemory) / float(mapObjMemoryFootprint), .30);
}

#endif // __linux__
