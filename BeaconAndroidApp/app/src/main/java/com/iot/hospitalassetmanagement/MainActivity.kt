package com.iot.hospitalassetmanagement

import android.Manifest
import android.app.Activity
import android.app.ActivityManager
import android.content.Context
import android.content.Intent
import android.content.SharedPreferences
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.safeDrawingPadding
import androidx.compose.foundation.layout.size
import androidx.compose.material3.Button
import androidx.compose.material3.LinearProgressIndicator
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.TextField
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.iot.hospitalassetmanagement.ui.theme.HospitalAssetManagementTheme


class MainActivity : ComponentActivity() {
    private val TAG = "tagJi"
    lateinit var sharedPref: SharedPreferences
    val permissions = arrayOf(
        Manifest.permission.BLUETOOTH_ADVERTISE,
        Manifest.permission.BLUETOOTH,
        Manifest.permission.BLUETOOTH_ADMIN,
        Manifest.permission.ACCESS_FINE_LOCATION,
        Manifest.permission.ACCESS_BACKGROUND_LOCATION,
        Manifest.permission.FOREGROUND_SERVICE
    )


    fun startBeaconService() {
        val res = checkPreConditions()
        if (!res) {
            Toast.makeText(this, "Please enable bluetooth and location", Toast.LENGTH_SHORT).show()
            return
        }
        val intent = Intent(this, BeaconTransmitterService::class.java)
        startForegroundService(intent)
    }

    fun stopBeaconService() {
        val intent = Intent(this, BeaconTransmitterService::class.java)
        val res = stopService(intent)
        if (res) {
            sharedPref.edit().putBoolean(Constants.BEACON_TRANSMITTER_SERVICE_RUNNING, false)
                .apply()
        }
    }

    fun isServiceRunning(): Boolean {
        return (getSystemService(ACTIVITY_SERVICE) as ActivityManager)
            .getRunningServices(Integer.MAX_VALUE)
            .any { it.service.className == BeaconTransmitterService::class.java.name }
    }

    fun checkPreConditions(): Boolean {
//        Check if bluetooth is enabled
        val bluetoothAdapter = android.bluetooth.BluetoothAdapter.getDefaultAdapter()
        if (bluetoothAdapter == null || !bluetoothAdapter.isEnabled) {
            Log.d(TAG, "Bluetooth is not enabled")
            return false
        }

//        check if location is on
        val locationManager =
            getSystemService(Context.LOCATION_SERVICE) as android.location.LocationManager
        if (!locationManager.isProviderEnabled(android.location.LocationManager.GPS_PROVIDER)) {
            Log.d(TAG, "Location is not enabled")
            return false
        }
        return true
    }

    private fun checkAndRequestPermissions(activity: Activity) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) { // Android 12+
            ActivityCompat.requestPermissions(
                activity,
                permissions,
                1
            )
        }
    }


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
//        checkAndRequestPermissions(this)
        ActivityCompat.requestPermissions(
            this,
            permissions,
            1
        )
        sharedPref = getSharedPreferences(Constants.SHARED_PREF_NAME, Context.MODE_PRIVATE)
        if (isServiceRunning()) {
            sharedPref.edit().putBoolean(Constants.BEACON_TRANSMITTER_SERVICE_RUNNING, true).apply()
        } else {
            sharedPref.edit().putBoolean(Constants.BEACON_TRANSMITTER_SERVICE_RUNNING, false)
                .apply()
        }
        val context = this
        enableEdgeToEdge()
        setContent {
            HospitalAssetManagementTheme {
                Scaffold(
                    modifier = Modifier
                        .fillMaxSize()
                        .safeDrawingPadding()
                ) { innerPadding ->
                    Column {
                        Text(
                            "IOT Asset Tracker - Beacon",
                            modifier = Modifier
                                .padding(0.dp)
                                .background(
                                    Color(0xFFB19FCA)
                                )
                                .fillMaxWidth(1f)
                                .padding(all = 16.dp)
                        )
                        MainScreen(
                            context,
                            ::startBeaconService,
                            ::stopBeaconService,
                            ::checkPreConditions,
                            sharedPref
                        )
                    }
                }
            }
        }
    }
}

@Composable
fun BeaconServiceLauncherScreen(
    context: Context,
    startBeaconService: () -> Unit,
    stopBeaconService: () -> Unit,
    checkPreConditions: () -> Boolean,
    sharedPreferences: SharedPreferences
) {
    val uuid = remember { mutableStateOf(Constants.DEFAULT_UUID) }
    var serviceRunning = remember {
        mutableStateOf(
            sharedPreferences.getBoolean(
                Constants.BEACON_TRANSMITTER_SERVICE_RUNNING,
                false
            )
        )
    }
    val handler = remember { Handler(Looper.getMainLooper()) }

    fun checkServiceRunning() {
        val isRunning =
            sharedPreferences.getBoolean(Constants.BEACON_TRANSMITTER_SERVICE_RUNNING, false)
        if (serviceRunning.value != isRunning) {
            serviceRunning.value = isRunning
        }
        handler.postDelayed(::checkServiceRunning, 5000) // Check every 5 seconds
    }
    DisposableEffect(Unit) {
        handler.post(::checkServiceRunning)
        onDispose {
            handler.removeCallbacks(::checkServiceRunning)
        }
    }

    val loading = remember { mutableStateOf(false) }
    Column(
        Modifier
            .fillMaxSize(),
        verticalArrangement = Arrangement.Center,
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        if (!loading.value) {
            if (serviceRunning.value) {
                Column {
                    Row(
                        verticalAlignment = Alignment.CenterVertically,
                        modifier = Modifier.padding(20.dp)
                    ) {
                        Image(
                            painter = painterResource(id = R.drawable.baseline_bluetooth_connected_24),
                            contentDescription = "Bluetooth icon",
                            modifier = Modifier
                                .padding(0.dp)
                                .size(30.dp)
                        )
                        Text("Beacon Service is running", modifier = Modifier.padding(0.dp))
                    }
                    Text(text = "UUID: ${uuid.value}", modifier = Modifier.padding(0.dp))
                }
                Button(onClick = {
                    stopBeaconService()
                    loading.value = true
                    handler.postDelayed({
                        loading.value = false
                    }, 4000)
                }, modifier = Modifier.padding(100.dp)) {
                    Text("Stop Beacon Service")
                }
            } else {
                TextField(
                    value = uuid.value,
                    onValueChange = { uuid.value = it },
                    label = { Text("UUID") },
                    modifier = Modifier.padding(10.dp, 100.dp)
                )
                Button(onClick = {
                    if (uuid.value.isEmpty() || uuid.value.length != 36) {
                        Toast.makeText(context, "Please enter a valid UUID", Toast.LENGTH_SHORT)
                            .show()
                        return@Button
                    }
                    sharedPreferences.edit().putString(Constants.UUID_KEY, uuid.value).apply()
                    startBeaconService()
                    loading.value = true
//                    start a timer to set loading to false after 5 seconds
                    handler.postDelayed({
                        loading.value = false
                    }, 4000)
                }, modifier = Modifier.padding(100.dp)) {
                    Text("Start Beacon Service")
                }
            }
        } else {
//          image from drawable resources
            LinearProgressIndicator(modifier = Modifier.padding(100.dp))
        }
    }
}

@Composable
fun MainScreen(
    context: Context,
    startBeaconService: () -> Unit,
    stopBeaconService: () -> Unit,
    checkPreConditions: () -> Boolean,
    sharedPreferences: SharedPreferences
) {
    Column {
        BeaconServiceLauncherScreen(
            context,
            startBeaconService,
            stopBeaconService,
            checkPreConditions,
            sharedPreferences
        )
    }
}

@Preview(showBackground = true, showSystemUi = true)
@Composable
fun GreetingPreview() {
    HospitalAssetManagementTheme {
        MainScreen(LocalContext.current, {}, {}, { false }, null!!)
    }
}