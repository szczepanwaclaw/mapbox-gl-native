package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;

import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.Random;

public class BigMarkerActivity extends AppCompatActivity {

    private MapView mMapView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_big_marker);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        final ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.setAccessToken(getString(R.string.mapbox_access_token));
        mMapView.onCreate(savedInstanceState);
        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull MapboxMap mapboxMap) {
                IconFactory iconFactory = IconFactory.getInstance(BigMarkerActivity.this);
                Icon[] icons = new Icon[]{
                        iconFactory.fromResource(R.drawable.ic_custom_marker_big),
                        iconFactory.fromResource(R.drawable.ic_custom_marker_normal),
                        iconFactory.fromResource(R.drawable.ic_custom_marker_small)
                };

                Random r = new Random();
                double lon = -180;
                int count = 10;
                for (int i = 0; i < count; i++) {
                    lon = lon - (180 / count)*2;
                    mapboxMap.addMarker(new MarkerOptions()
                                    .position(new LatLng(0, lon))
                                    .icon(icons[r.nextInt(icons.length)])
                                    .snippet("Snippet " + i)
                                    .title("Title " + i)
                    );
                }

                lon = -180;
                count = 20;
                for (int i = 0; i < count; i++) {
                    lon = lon - (180 / count)*2;
                    mapboxMap.addMarker(new MarkerOptions()
                                    .position(new LatLng(20, lon))
                                    .snippet("Snippet " + i)
                                    .title("Title " + i)
                    );
                }
            }
        });
    }

    @Override
    public void onResume() {
        super.onResume();
        mMapView.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
        mMapView.onPause();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mMapView.onSaveInstanceState(outState);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mMapView.onDestroy();
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
        mMapView.onLowMemory();
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }
}