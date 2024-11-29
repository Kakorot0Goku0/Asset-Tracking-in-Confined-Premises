package com.iot.hospitalassetmanagement

import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.app.Service
import android.bluetooth.le.AdvertiseCallback
import android.bluetooth.le.AdvertiseSettings
import android.content.Context
import android.content.Intent
import android.content.SharedPreferences
import android.content.pm.ServiceInfo
import android.os.Build
import android.os.IBinder
import android.util.Log
import androidx.core.app.NotificationCompat
import androidx.core.app.ServiceCompat
import org.altbeacon.beacon.Beacon
import org.altbeacon.beacon.BeaconParser
import org.altbeacon.beacon.BeaconTransmitter


class BeaconTransmitterService : Service() {
    private val CHANNEL_ID = "beacon_service_channel"
    lateinit var beaconTransmitter: BeaconTransmitter
    private val TAG = "tagJi"
    lateinit var sharedPref: SharedPreferences;

    override fun onCreate() {
        super.onCreate()
        sharedPref = getSharedPreferences(Constants.SHARED_PREF_NAME, Context.MODE_PRIVATE)
        createNotificationChannel()
    }

    private fun createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val serviceChannel = NotificationChannel(
                CHANNEL_ID,
                "Beacon Service Channel",
                NotificationManager.IMPORTANCE_DEFAULT
            )
            val manager = getSystemService(NotificationManager::class.java)
            manager.createNotificationChannel(serviceChannel)
        }
    }

    private fun startForeground() {
        val uuid = sharedPref.getString(Constants.UUID_KEY, Constants.DEFAULT_UUID).toString()
        val notification = NotificationCompat.Builder(this, CHANNEL_ID)
            .setContentTitle("Beacon Started")
            .setContentText("Beacon is running in the background")
            .setSmallIcon(R.drawable.ic_launcher_background) // Ensure you have a valid icon
            .setOngoing(true)
            .setContentIntent(
                PendingIntent.getActivity(
                    this,
                    0,
                    Intent(this, MainActivity::class.java),
                    PendingIntent.FLAG_IMMUTABLE
                )
            )
            .build()
        ServiceCompat.startForeground(
            this, 1, notification, if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                ServiceInfo.FOREGROUND_SERVICE_TYPE_LOCATION
            } else {
                0
            }
        )
        startBeacon(uuid, { running ->
            if (running) {
                sharedPref.edit().putBoolean(Constants.BEACON_TRANSMITTER_SERVICE_RUNNING, true)
                    .apply()
                Log.d(TAG, "Beacon is running from background")
            } else {
                sharedPref.edit().putBoolean(Constants.BEACON_TRANSMITTER_SERVICE_RUNNING, false)
                    .apply()
                Log.d(TAG, "Beacon is not running from foreground")
            }
        })
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        startForeground()
        return START_STICKY
    }

    override fun onBind(intent: Intent?): IBinder? {
        return null
    }

    /*      Beacon code start here      */
    private fun startBeacon(uuid: String, setRunning: (Boolean) -> Unit) {
        val beacon = Beacon.Builder()
            .setId1(uuid)
            .setId2("1")
            .setId3("2")
            .setManufacturer(0x004C) // Radius Networks.  Change this for other beacon layouts
            .setTxPower(-59)
            .setDataFields(listOf())
            .build()

// Change the layout below for other beacon types
        val beaconParser = BeaconParser()
            .setBeaconLayout("m:2-3=0215,i:4-19,i:20-21,i:22-23,p:24-24")
        beaconTransmitter = BeaconTransmitter(applicationContext, beaconParser)
        beaconTransmitter.startAdvertising(beacon, object : AdvertiseCallback() {
            override fun onStartFailure(errorCode: Int) {
                Log.e(TAG, "Advertisement start failed with code: $errorCode")
                setRunning(false)
            }

            override fun onStartSuccess(settingsInEffect: AdvertiseSettings) {
                Log.i(TAG, "Advertisement start succeeded.")
                setRunning(true)
            }
        })
    }

    override fun onDestroy() {
        super.onDestroy()
        beaconTransmitter.stopAdvertising()
        sharedPref.edit().putBoolean(Constants.BEACON_TRANSMITTER_SERVICE_RUNNING, false).apply()
    }
}