package com.example.libperm

import android.app.Activity
import android.content.Intent
import android.content.pm.PackageManager.PERMISSION_GRANTED
import android.net.Uri
import android.provider.Settings
import android.util.Log
import android.widget.Toast
import java.io.File


//import android.content.pm.PackageManager
//import org.xmlpull.v1.XmlPullParserException
//import sun.security.pkcs11.wrapper.Functions.getAttributeName
//import org.xmlpull.v1.XmlPullParser
//import android.content.res.XmlResourceParser
//import sun.invoke.util.VerifyAccess.getPackageName
//import android.content.res.AssetManager
//fun getListOfPermissions(context: Context): String {
//    var _permissions = ""
//
//    try {
//        val _am = context.createPackageContext(context.getPackageName(), 0).getAssets()
//        val _xmlParser = _am.openXmlResourceParser(0, "AndroidManifest.xml")
//        var _eventType = _xmlParser.getEventType()
//        while (_eventType != XmlPullParser.END_DOCUMENT) {
//            if (_eventType == XmlPullParser.START_TAG && "uses-permission" == _xmlParser.getName()) {
//                for (i in 0 until _xmlParser.getAttributeCount()) {
//                    if (_xmlParser.getAttributeName(i) == "name") {
//                        _permissions += _xmlParser.getAttributeValue(i) + "\n"
//                    }
//                }
//            }
//            _eventType = _xmlParser.nextToken()
//        }
//        _xmlParser.close() // Pervents memory leak.
//    } catch (exception: XmlPullParserException) {
//        exception.printStackTrace()
//    } catch (exception: PackageManager.NameNotFoundException) {
//        exception.printStackTrace()
//    } catch (exception: IOException) {
//        exception.printStackTrace()
//    }
//
//    return _permissions
//}



class PermissionManager(
    private val activity: Activity
) {

    val permission = PermissionData()
    val protection = permission.dataByProtection
    var data = protection.none
    var permissionLevel: Int = permission.protectionLevelNone
    var page = 0
    var previousSize = 0
    var size = 4
    var hasNextPage = true
    var total = 0
    var increaseBy = 0

    inner class Permissions(private vararg val permissionArray: String) {

        private fun stage1(permission: String, id: Int, permissionArray: List<String>) {
            when (permissionArray[0]) {
                "android", "com" -> stage2(permission, id, permissionArray)
                else -> throw Exception("request: Unknown permission type: ${permissionArray[0]} ($permission)")
            }
        }

        private fun stage2(permission: String, id: Int, permissionArray: List<String>) {
            when (permissionArray[1]) {
                "permission", "permission-group" -> activity.requestPermissions(arrayOf(permission), id)
                "settings", "android" -> stage3(permission, id, permissionArray)
                else -> throw Exception("request: Unknown permission type: ${permissionArray[1]} ($permission)")
            }
        }

        private fun stage3(permission: String, id: Int, permissionArray: List<String>) {
            when (permissionArray[2]) {
                "action" -> {
                    val URI = Uri.parse("package:${activity.packageName}")
                    activity.startActivityForResult(Intent(permission, URI), 1)
                }
                "launcher", "alarm", "voicemail" -> stage4(permission, id, permissionArray)
                else -> throw Exception("request: Unknown permission type: ${permissionArray[2]} ($permission)")
            }
        }

        private fun stage4(permission: String, id: Int, permissionArray: List<String>) {
            when (permissionArray[3]) {
                "permission", "permission-group" -> activity.requestPermissions(arrayOf(permission), id)
                else -> throw Exception("request: Unknown permission type: ${permissionArray[3]} ($permission)")
            }
        }

        fun request(permission: String, id: Int) {
            stage1(permission, id, permission.split('.'))
        }

        fun requestAllRemaining(): Boolean {
            for ((index, it) in permissionArray.withIndex()) {
                if (!check(it)) {
                    request(it, index)
//                    if (!check(it)) return false
                }
            }
            return true
        }

        fun check(permission: String): Boolean {
            val permissionArray = permission.split('.')
            return when (permissionArray[0]) {
                "android" -> when (permissionArray[1]) {
                    "permission", "permission-group" -> activity.checkSelfPermission(permission) == PERMISSION_GRANTED
                    "settings" -> when (permissionArray[2]) {
                        "action" -> when (permissionArray[3]) {
                            "MANAGE_OVERLAY_PERMISSION" -> Settings.canDrawOverlays(activity.applicationContext)
                            else -> throw Exception("check: Unknown permission: ${permissionArray[3]} ($permission)")
                        }
                        else -> throw Exception("check: Unknown permission type: ${permissionArray[2]} ($permission)")
                    }
                    else -> throw Exception("check: Unknown permission type: ${permissionArray[1]} ($permission)")
                }
                else -> throw Exception("check: Unknown permission type: ${permissionArray[0]} ($permission)")
            }
        }

        fun checkAll(): Boolean {
            permissionArray.forEach {
                if (!check(it)) return false
            }
            return true
        }

//        fun getpermissions(pkg: String) {
//            val ph = PackageHunter(activity.applicationContext)
//            val pkgprm = ph.getPermissionForPkg(pkg)
//            pkgprm.forEach {
//                Log.i("pkgprm", "pkgprm contains $it")
//            }
//        }

    }
}

fun String.longestLine(): String = this.longestSubString('\n')
fun String.longestWord(): String = this.longestSubString(' ')

fun String.longestSubString(delimiter: Char): String {
    var longest = 0
    var line: String = ""
    this.split(delimiter).forEach {
        it.trim()
        if (it.length > longest) {
            longest = it.length
            line = it
        }
    }
    return line
}
