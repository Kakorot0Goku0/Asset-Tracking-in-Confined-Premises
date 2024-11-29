package com.iot.hospitalassetmanagement

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.util.Log

class BeaconStopBroadcastReceiver : BroadcastReceiver() {
    val TAG = "tagJi"
    override fun onReceive(context: Context?, intent: Intent?) {
        Log.d(TAG, "Broadcast received: " + intent?.action.toString())
        if (intent?.action == "com.iot.hospitalassetmanagement.STOP_BEACON") {
            val beaconTransmitterServiceIntent =
                Intent(context, BeaconTransmitterService::class.java)
            context?.stopService(beaconTransmitterServiceIntent)
        }
    }
}