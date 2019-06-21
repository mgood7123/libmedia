package com.example.libperm

import android.Manifest
import android.os.Build

class PermissionData {
    val protectionLevelNone = 0
    val protectionLevelNormal = 1
    val protectionLevelSignature = 2
    val protectionLevelPrivileged = 3
    val protectionLevelDangerous = 4

    fun permissionLevelToString(level: Int): String = when (level) {
        protectionLevelNone -> "None"
        protectionLevelNormal -> "Normal"
        protectionLevelSignature -> "Signature"
        protectionLevelPrivileged -> "Privileged"
        protectionLevelDangerous -> "Dangerous"
        else -> "Unknown Permission Level"
    }

    data class Permission(
        val name: String,
        val nameEnglish: String,
        val addedInApiLevel: Int?,
        val deprecationInApiLevel: Int?,
        val description: String,
        val protectionLevels: IntArray
    )

    data class ByProtectionLevel(
        val none: MutableList<Permission> = mutableListOf<Permission>(),
        val normal: MutableList<Permission> = mutableListOf<Permission>(),
        val signature: MutableList<Permission> = mutableListOf<Permission>(),
        val privileged: MutableList<Permission> = mutableListOf<Permission>(),
        val dangerous: MutableList<Permission> = mutableListOf<Permission>()
    )

    val data = mutableListOf<Permission>()
    val dataByProtection = ByProtectionLevel()

    private fun add(
        name: String,
        addedInApiLevel: Int?,
        deprecationInApiLevel: Int?,
        description: String,
        vararg protectionLevel: Int
    ) {
        data.add(
            Permission(
                name,
                name.substringAfterLast('.').replace('_', ' '),
                addedInApiLevel,
                deprecationInApiLevel,
                description,
                protectionLevel
            )
        )
        protectionLevel.forEach {
            when (it) {
                protectionLevelNone -> dataByProtection.none.add(
                    Permission(
                        name,
                        name.substringAfterLast('.').replace('_', ' '),
                        addedInApiLevel,
                        deprecationInApiLevel,
                        description,
                        protectionLevel
                    )
                )
                protectionLevelNormal -> dataByProtection.normal.add(
                    Permission(
                        name,
                        name.substringAfterLast('.').replace('_', ' '),
                        addedInApiLevel,
                        deprecationInApiLevel,
                        description,
                        protectionLevel
                    )
                )
                protectionLevelSignature -> dataByProtection.signature.add(
                    Permission(
                        name,
                        name.substringAfterLast('.').replace('_', ' '),
                        addedInApiLevel,
                        deprecationInApiLevel,
                        description,
                        protectionLevel
                    )
                )
                protectionLevelPrivileged -> dataByProtection.privileged.add(
                    Permission(
                        name,
                        name.substringAfterLast('.').replace('_', ' '),
                        addedInApiLevel,
                        deprecationInApiLevel,
                        description,
                        protectionLevel
                    )
                )
                protectionLevelDangerous -> dataByProtection.dangerous.add(
                    Permission(
                        name,
                        name.substringAfterLast('.').replace('_', ' '),
                        addedInApiLevel,
                        deprecationInApiLevel,
                        description,
                        protectionLevel
                    )
                )
            }
        }
    }

    init {
//        add(
//            Manifest.permission.ACCEPT_HANDOVER,
//            28,
//            null,
//            """Allows a calling app to continue a call which was started in another app. An example is a video calling app that wants to continue a voice call on the user's mobile network.
//
//When the handover of a call from one app to another takes place, there are two devices which are involved in the handover; the initiating and receiving devices. The initiating device is where the request to handover the call was started, and the receiving device is where the handover request is confirmed by the other party.
//
//This permission protects access to the TelecomManager.acceptHandover(Uri, int, PhoneAccountHandle) which the receiving side of the handover uses to accept a handover.""",
//            protectionLevelDangerous
//        )
        add(
            // Android Q SDK not released
//            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q)
//                Manifest.permission.ACCESS_BACKGROUND_LOCATION
//            else
                "android.permission.ACCESS_BACKGROUND_LOCATION",
            29,
            null,
            """Allows an app to access location in the background. If you are requesting this, you should also request ACCESS_FINE_LOCATION. Requesting this by itself is not sufficient to give you location access. """,
            protectionLevelDangerous
        )
        add(
            Manifest.permission.ACCESS_CHECKIN_PROPERTIES,
            1,
            null,
            """Allows read/write access to the "properties" table in the checkin database, to change values that get uploaded.

Not for use by third-party applications.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.ACCESS_COARSE_LOCATION,
            1,
            null,
            """Allows an app to access approximate location. Alternatively, you might want ACCESS_FINE_LOCATION. """,
            protectionLevelDangerous
        )
        add(
            Manifest.permission.ACCESS_FINE_LOCATION,
            1,
            null,
            """Allows an app to access precise location. Alternatively, you might want ACCESS_COARSE_LOCATION. """,
            protectionLevelDangerous
        )
        add(
            Manifest.permission.ACCESS_LOCATION_EXTRA_COMMANDS,
            1,
            null,
            """Allows an application to access extra location provider commands. """,
            protectionLevelNormal
        )
        add(
            // Android Q SDK not released
//            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q)
//                Manifest.permission.ACCESS_MEDIA_LOCATION
//            else
                "android.permission.ACCESS_MEDIA_LOCATION",
            29,
            null,
            """Allows an application to access any geographic locations persisted in the user's shared collection.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.ACCESS_NETWORK_STATE,
            1,
            null,
            """Allows applications to access information about networks. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.ACCESS_NOTIFICATION_POLICY,
            23,
            null,
            """Marker permission for applications that wish to access notification policy. This permission is not supported on managed profiles.""",
            protectionLevelNormal
        )
        add(
            Manifest.permission.ACCESS_WIFI_STATE,
            1,
            null,
            """Allows applications to access information about Wi-Fi networks. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.ACCOUNT_MANAGER,
            5,
            null,
            """Allows applications to call into AccountAuthenticators. """,
            protectionLevelNone
        )
        add(
            // Android Q SDK not released
//            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q)
//                Manifest.permission.ACTIVITY_RECOGNITION
//            else
                "android.permission.ACTIVITY_RECOGNITION",
            29,
            null,
            """Allows an application to recognize physical activity. """,
            protectionLevelDangerous
        )
        add(
            Manifest.permission.ADD_VOICEMAIL,
            14,
            null,
            """Allows an application to add voicemails into the system. """,
            protectionLevelDangerous
        )
        add(
            Manifest.permission.ANSWER_PHONE_CALLS, 26, null, """Allows the app to answer an incoming phone call.

""", protectionLevelDangerous
        )
        add(
            Manifest.permission.BATTERY_STATS,
            26,
            null,
            """Allows an application to collect battery statistics""",
            protectionLevelNone
        )
        add(
            Manifest.permission.BIND_ACCESSIBILITY_SERVICE,
            16,
            null,
            """Must be required by an AccessibilityService, to ensure that only the system can bind to it. """,
            protectionLevelSignature
        )
        add(
            Manifest.permission.BIND_APPWIDGET,
            3,
            null,
            """Allows an application to tell the AppWidget service which application can access AppWidget's data. The normal user flow is that a user picks an AppWidget to go into a particular host, thereby giving that host application access to the private data from the AppWidget app. An application that has this permission should honor that contract.

Not for use by third-party applications.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.BIND_AUTOFILL_SERVICE,
            26,
            null,
            """Must be required by a AutofillService, to ensure that only the system can bind to it. """,
            protectionLevelSignature
        )
        add(
            // Android Q SDK not released
//            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q)
//                Manifest.permission.BIND_CALL_REDIRECTION_SERVICE
//            else
                "android.permission.BIND_CALL_REDIRECTION_SERVICE",
            29,
            null,
            """Must be required by a CallRedirectionService, to ensure that only the system can bind to it. """,
            protectionLevelSignature,
            protectionLevelPrivileged
        )
        add(
            // Android Q SDK not released
//            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q)
//                Manifest.permission.BIND_CARRIER_MESSAGING_CLIENT_SERVICE
//            else
                "android.permission.BIND_CARRIER_MESSAGING_CLIENT_SERVICE",
            29,
            null,
            """A subclass of CarrierMessagingClientService must be protected with this permission.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.BIND_CARRIER_MESSAGING_SERVICE, 22, 23, """This constant was deprecated in API level 23.
Use BIND_CARRIER_SERVICES instead """, protectionLevelNone
        )
        add(
            Manifest.permission.BIND_CARRIER_SERVICES,
            23,
            null,
            """The system process that is allowed to bind to services in carrier apps will have this permission. Carrier apps should use this permission to protect their services that only the system is allowed to bind to. """,
            protectionLevelSignature,
            protectionLevelPrivileged
        )
        add(
            Manifest.permission.BIND_CHOOSER_TARGET_SERVICE,
            23,
            null,
            """Must be required by a ChooserTargetService, to ensure that only the system can bind to it. """,
            protectionLevelSignature
        )
        add(
            Manifest.permission.BIND_CONDITION_PROVIDER_SERVICE,
            24,
            null,
            """Must be required by a ConditionProviderService, to ensure that only the system can bind to it. """,
            protectionLevelSignature
        )
        add(
            Manifest.permission.BIND_DEVICE_ADMIN,
            8,
            null,
            """Must be required by device administration receiver, to ensure that only the system can interact with it. """,
            protectionLevelSignature
        )
        add(
            Manifest.permission.BIND_DREAM_SERVICE,
            21,
            null,
            """Must be required by an DreamService, to ensure that only the system can bind to it. """,
            protectionLevelSignature
        )
        add(
            Manifest.permission.BIND_INCALL_SERVICE,
            23,
            null,
            """Must be required by a InCallService, to ensure that only the system can bind to it. """,
            protectionLevelSignature,
            protectionLevelPrivileged
        )
        add(
            Manifest.permission.BIND_INPUT_METHOD,
            3,
            null,
            """Must be required by an InputMethodService, to ensure that only the system can bind to it. """,
            protectionLevelSignature
        )
        add(
            Manifest.permission.BIND_MIDI_DEVICE_SERVICE,
            23,
            null,
            """Must be required by an MidiDeviceService, to ensure that only the system can bind to it. """,
            protectionLevelSignature
        )
        add(
            Manifest.permission.BIND_NFC_SERVICE,
            19,
            null,
            """Must be required by a HostApduService or OffHostApduService to ensure that only the system can bind to it. """,
            protectionLevelSignature
        )
        add(
            Manifest.permission.BIND_NOTIFICATION_LISTENER_SERVICE,
            18,
            null,
            """Must be required by an NotificationListenerService, to ensure that only the system can bind to it. """,
            protectionLevelSignature
        )
        add(
            Manifest.permission.BIND_PRINT_SERVICE,
            19,
            null,
            """Must be required by a PrintService, to ensure that only the system can bind to it.""",
            protectionLevelSignature
        )
        add(
            Manifest.permission.BIND_QUICK_SETTINGS_TILE,
            24,
            null,
            """Allows an application to bind to third party quick settings tiles.

Should only be requested by the System, should be required by TileService declarations.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.BIND_REMOTEVIEWS,
            11,
            null,
            """Must be required by a RemoteViewsService, to ensure that only the system can bind to it. """,
            protectionLevelNone
        )
        add(
            Manifest.permission.BIND_SCREENING_SERVICE,
            24,
            null,
            """Must be required by a CallScreeningService, to ensure that only the system can bind to it. """,
            protectionLevelSignature,
            protectionLevelPrivileged
        )
        add(
            Manifest.permission.BIND_TELECOM_CONNECTION_SERVICE,
            23,
            null,
            """Must be required by a ConnectionService, to ensure that only the system can bind to it. """,
            protectionLevelSignature,
            protectionLevelPrivileged
        )
        add(
            Manifest.permission.BIND_TEXT_SERVICE,
            14,
            null,
            """Must be required by a TextService (e.g. SpellCheckerService) to ensure that only the system can bind to it. """,
            protectionLevelSignature
        )
        add(
            Manifest.permission.BIND_TV_INPUT,
            21,
            null,
            """Must be required by a TvInputService to ensure that only the system can bind to it.""",
            protectionLevelSignature,
            protectionLevelPrivileged
        )
        add(
            Manifest.permission.BIND_VISUAL_VOICEMAIL_SERVICE,
            26,
            null,
            """Must be required by a link VisualVoicemailService to ensure that only the system can bind to it. """,
            protectionLevelSignature,
            protectionLevelPrivileged
        )
        add(
            Manifest.permission.BIND_VOICE_INTERACTION,
            21,
            null,
            """Must be required by a VoiceInteractionService, to ensure that only the system can bind to it. """,
            protectionLevelSignature
        )
        add(
            Manifest.permission.BIND_VPN_SERVICE,
            14,
            null,
            """Must be required by a VpnService, to ensure that only the system can bind to it. """,
            protectionLevelSignature
        )
        add(
            Manifest.permission.BIND_VR_LISTENER_SERVICE,
            24,
            null,
            """Must be required by an VrListenerService, to ensure that only the system can bind to it. """,
            protectionLevelSignature
        )
        add(
            Manifest.permission.BIND_WALLPAPER,
            8,
            null,
            """Must be required by a WallpaperService, to ensure that only the system can bind to it. """,
            protectionLevelSignature,
            protectionLevelPrivileged
        )
        add(
            Manifest.permission.BLUETOOTH,
            1,
            null,
            """Allows applications to connect to paired bluetooth devices. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.BLUETOOTH_ADMIN,
            1,
            null,
            """Allows applications to discover and pair bluetooth devices. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.BLUETOOTH_PRIVILEGED,
            19,
            null,
            """Allows applications to pair bluetooth devices without user interaction, and to allow or disallow phonebook access or message access. This is not available to third party applications.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.BODY_SENSORS,
            20,
            null,
            """Allows an application to access data from sensors that the user uses to measure what is happening inside his/her body, such as heart rate. """,
            protectionLevelDangerous
        )
        add(
            Manifest.permission.BROADCAST_PACKAGE_REMOVED,
            1,
            null,
            """Allows an application to broadcast a notification that an application package has been removed.

Not for use by third-party applications.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.BROADCAST_SMS,
            2,
            null,
            """Allows an application to broadcast an SMS receipt notification.

Not for use by third-party applications.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.BROADCAST_STICKY,
            1,
            null,
            """Allows an application to broadcast sticky intents. These are broadcasts whose data is held by the system after being finished, so that clients can quickly retrieve that data without having to wait for the next broadcast. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.BROADCAST_WAP_PUSH,
            2,
            null,
            """Allows an application to broadcast a WAP PUSH receipt notification.

Not for use by third-party applications.""",
            protectionLevelNone
        )
        add(
            // Android Q SDK not released
//            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q)
//                Manifest.permission.CALL_COMPANION_APP
//            else
                "android.permission.CALL_COMPANION_APP",
            29,
            null,
            """Allows an app which implements the InCallService API to be eligible to be enabled as a calling companion app. This means that the Telecom framework will bind to the app's InCallService implementation when there are calls active. The app can use the InCallService API to view information about calls on the system and control these calls. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.CALL_PHONE,
            1,
            null,
            """Allows an application to initiate a phone call without going through the Dialer user interface for the user to confirm the call. """,
            protectionLevelDangerous
        )
        add(
            Manifest.permission.CALL_PRIVILEGED,
            1,
            null,
            """Allows an application to call any phone number, including emergency numbers, without going through the Dialer user interface for the user to confirm the call being placed.

Not for use by third-party applications.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.CAMERA,
            1,
            null,
            """Required to be able to access the camera device.

This will automatically enforce the uses-feature manifest element for all camera features. If you do not require all camera features or can properly operate if a camera is not available, then you must modify your manifest as appropriate in order to install on devices that don't support all camera features.""",
            protectionLevelDangerous
        )
        add(
            Manifest.permission.CAPTURE_AUDIO_OUTPUT,
            19,
            null,
            """Allows an application to capture audio output. Use the CAPTURE_MEDIA_OUTPUT permission if only the USAGE_UNKNOWN), USAGE_MEDIA) or USAGE_GAME) usages are intended to be captured.

Not for use by third-party applications.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.CHANGE_COMPONENT_ENABLED_STATE,
            1,
            null,
            """Allows an application to change whether an application component (other than its own) is enabled or not.

Not for use by third-party applications.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.CHANGE_CONFIGURATION,
            1,
            null,
            """Allows an application to modify the current configuration, such as locale.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.CHANGE_NETWORK_STATE,
            1,
            null,
            """Allows applications to change network connectivity state. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.CHANGE_WIFI_MULTICAST_STATE,
            4,
            null,
            """Allows applications to enter Wi-Fi Multicast mode. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.CHANGE_WIFI_STATE,
            1,
            null,
            """Allows applications to change Wi-Fi connectivity state. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.CLEAR_APP_CACHE,
            1,
            null,
            """Allows an application to clear the caches of all installed applications on the device. """,
            protectionLevelSignature,
            protectionLevelPrivileged
        )
        add(
            Manifest.permission.CONTROL_LOCATION_UPDATES,
            1,
            null,
            """Allows enabling/disabling location update notifications from the radio.

Not for use by third-party applications.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.DELETE_CACHE_FILES,
            1,
            null,
            """Old permission for deleting an app's cache files, no longer used, but signals for us to quietly ignore calls instead of throwing an exception.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.DELETE_PACKAGES,
            1,
            null,
            """Allows an application to delete packages.

Not for use by third-party applications.

Starting in Build.VERSION_CODES.N, user confirmation is requested when the application deleting the package is not the same application that installed the package.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.DIAGNOSTIC, 1, null, """Allows applications to RW to diagnostic resources.

Not for use by third-party applications.""", protectionLevelNone
        )
        add(
            Manifest.permission.DISABLE_KEYGUARD,
            1,
            null,
            """Allows applications to disable the keyguard if it is not secure. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.DUMP,
            1,
            null,
            """Allows an application to retrieve state dump information from system services.

Not for use by third-party applications.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.EXPAND_STATUS_BAR,
            1,
            null,
            """Allows an application to expand or collapse the status bar. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.FACTORY_TEST,
            1,
            null,
            """Run as a manufacturer test application, running as the root user. Only available when the device is running in manufacturer test mode.

Not for use by third-party applications.""",
            protectionLevelNone
        )
//        add(
//            Manifest.permission.FOREGROUND_SERVICE,
//            28,
//            null,
//            """Allows a regular application to use Service.startForeground. """,
//            protectionLevelNormal
//        )
        add(
            Manifest.permission.GET_ACCOUNTS,
            1,
            null,
            """Allows access to the list of accounts in the Accounts Service.

Note: Beginning with Android 6.0 (API level 23), if an app shares the signature of the authenticator that manages an account, it does not need "GET_ACCOUNTS" permission to read information about that account. On Android 5.1 and lower, all apps need "GET_ACCOUNTS" permission to read information about any account.""",
            protectionLevelDangerous
        )
        add(
            Manifest.permission.GET_ACCOUNTS_PRIVILEGED,
            23,
            null,
            """Allows access to the list of accounts in the Accounts Service.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.GET_PACKAGE_SIZE,
            1,
            null,
            """Allows an application to find out the space used by any package. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.GET_TASKS, 1, 21, """This constant was deprecated in API level 21.
No longer enforced. """, protectionLevelNone
        )
        add(
            Manifest.permission.GLOBAL_SEARCH,
            4,
            null,
            """This permission can be used on content providers to allow the global search system to access their data. Typically it used when the provider has some permissions protecting it (which global search would not be expected to hold), and added as a read-only permission to the path in the provider where global search queries are performed. This permission can not be held by regular applications; it is used by applications to protect themselves from everyone else besides global search.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.INSTALL_LOCATION_PROVIDER,
            4,
            null,
            """Allows an application to install a location provider into the Location Manager.

Not for use by third-party applications.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.INSTALL_PACKAGES, 4, null, """Allows an application to install packages.

Not for use by third-party applications.""", protectionLevelNone
        )
        add(
            Manifest.permission.INSTALL_SHORTCUT,
            19,
            null,
            """Allows an application to install a shortcut in Launcher.

In Android O (API level 26) and higher, the INSTALL_SHORTCUT broadcast no longer has any effect on your app because it's a private, implicit broadcast. Instead, you should create an app shortcut by using the requestPinShortcut() method from the ShortcutManager class. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.INSTANT_APP_FOREGROUND_SERVICE,
            26,
            null,
            """Allows an instant app to create foreground services.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.INTERNET,
            1,
            null,
            """Allows applications to open network sockets. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.KILL_BACKGROUND_PROCESSES,
            8,
            null,
            """Allows an application to call ActivityManager.killBackgroundProcesses(String). """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.LOCATION_HARDWARE,
            18,
            null,
            """Allows an application to use location features in hardware, such as the geofencing api.

Not for use by third-party applications.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.MANAGE_DOCUMENTS,
            19,
            null,
            """Allows an application to manage access to documents, usually as part of a document picker.

This permission should only be requested by the platform document management app. This permission cannot be granted to third-party apps.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.MANAGE_OWN_CALLS,
            26,
            null,
            """Allows a calling application which manages it own calls through the self-managed ConnectionService APIs. See PhoneAccount.CAPABILITY_SELF_MANAGED for more information on the self-managed ConnectionService APIs. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.MASTER_CLEAR,
            1,
            null,
            """Not for use by third-party applications.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.MEDIA_CONTENT_CONTROL,
            19,
            null,
            """Allows an application to know what content is playing and control its playback.

Not for use by third-party applications due to privacy of media consumption""",
            protectionLevelNone
        )
        add(
            Manifest.permission.MODIFY_AUDIO_SETTINGS,
            1,
            null,
            """Allows an application to modify global audio settings. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.MODIFY_PHONE_STATE,
            1,
            null,
            """Allows modification of the telephony state - power on, mmi, etc. Does not include placing calls.

Not for use by third-party applications.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.MOUNT_FORMAT_FILESYSTEMS,
            3,
            null,
            """Allows formatting file systems for removable storage.

Not for use by third-party applications.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.MOUNT_UNMOUNT_FILESYSTEMS,
            1,
            null,
            """Allows mounting and unmounting file systems for removable storage.

Not for use by third-party applications.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.NFC,
            9,
            null,
            """Allows applications to perform I/O operations over NFC. """,
            protectionLevelNormal
        )
//        add(
//            Manifest.permission.NFC_TRANSACTION_EVENT,
//            28,
//            null,
//            """Allows applications to receive NFC transaction events. """,
//            protectionLevelNormal
//        )
        add(
            Manifest.permission.PACKAGE_USAGE_STATS,
            23,
            null,
            """Allows an application to collect component usage statistics

Declaring the permission implies intention to use the API and the user of the device can grant permission through the Settings application.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.PERSISTENT_ACTIVITY, 1, 15, """This constant was deprecated in API level 15.
This functionality will be removed in the future; please do not use. Allow an application to make its activities persistent. """
        )
        add(
            Manifest.permission.PROCESS_OUTGOING_CALLS,
            1,
            29,
            """ This constant was deprecated in API level Q.
Applications should use CallRedirectionService instead of the Intent.ACTION_NEW_OUTGOING_CALL broadcast.

Allows an application to see the number being dialed during an outgoing call with the option to redirect the call to a different number or abort the call altogether. """,
            protectionLevelDangerous
        )
        add(
            Manifest.permission.READ_CALENDAR,
            1,
            null,
            """Allows an application to read the user's calendar data. """,
            protectionLevelDangerous
        )
        add(
            Manifest.permission.READ_CALL_LOG,
            16,
            null,
            """Allows an application to read the user's call log.

Note: If your app uses the READ_CONTACTS permission and both your minSdkVersion and targetSdkVersion values are set to 15 or lower, the system implicitly grants your app this permission. If you don't need this permission, be sure your targetSdkVersion is 16 or higher.""",
            protectionLevelDangerous
        )
        add(
            Manifest.permission.READ_CONTACTS,
            1,
            null,
            """Allows an application to read the user's contacts data. """,
            protectionLevelDangerous
        )
        add(
            Manifest.permission.READ_EXTERNAL_STORAGE,
            16,
            null,
            """Allows an application to read from external storage.

Any app that declares the WRITE_EXTERNAL_STORAGE permission is implicitly granted this permission.

This permission is enforced starting in API level 19. Before API level 19, this permission is not enforced and all apps still have access to read from external storage. You can test your app with the permission enforced by enabling Protect USB storage under Developer options in the Settings app on a device running Android 4.1 or higher.

Also starting in API level 19, this permission is not required to read/write files in your application-specific directories returned by Context.getExternalFilesDir(String) and Context.getExternalCacheDir().

Note: If both your minSdkVersion and targetSdkVersion values are set to 3 or lower, the system implicitly grants your app this permission. If you don't need this permission, be sure your targetSdkVersion is 4 or higher.""",
            protectionLevelDangerous
        )
//        add(Manifest.permission.READ_FRAME_BUFFER) // removed in API 29 and no longer available in the docs
        add(
            Manifest.permission.READ_INPUT_STATE, 1, 16, """ This constant was deprecated in API level 16.
The API that used this permission has been removed.

Allows an application to retrieve the current state of keys and switches.

Not for use by third-party applications.""", protectionLevelNone
        )
        add(
            Manifest.permission.READ_LOGS,
            1,
            null,
            """Allows an application to read the low-level system log files.

Not for use by third-party applications, because Log entries can contain the user's private information.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.READ_PHONE_NUMBERS,
            26,
            null,
            """Allows read access to the device's phone number(s). This is a subset of the capabilities granted by READ_PHONE_STATE but is exposed to instant applications. """,
            protectionLevelDangerous
        )
        add(
            Manifest.permission.READ_PHONE_STATE,
            1,
            null,
            """Allows read only access to phone state, including the phone number of the device, current cellular network information, the status of any ongoing calls, and a list of any PhoneAccounts registered on the device.

Note: If both your minSdkVersion and targetSdkVersion values are set to 3 or lower, the system implicitly grants your app this permission. If you don't need this permission, be sure your targetSdkVersion is 4 or higher. """,
            protectionLevelDangerous
        )
        add(
            Manifest.permission.READ_SMS,
            1,
            null,
            """Allows an application to read SMS messages. """,
            protectionLevelDangerous
        )
        add(
            Manifest.permission.READ_SYNC_SETTINGS,
            1,
            null,
            """Allows applications to read the sync settings. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.READ_SYNC_STATS,
            1,
            null,
            """Allows applications to read the sync stats. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.READ_VOICEMAIL,
            21,
            null,
            """Allows an application to read voicemails in the system.""",
            protectionLevelSignature,
            protectionLevelPrivileged
        )
        add(
            Manifest.permission.REBOOT, 1, null, """Required to be able to reboot the device.

Not for use by third-party applications.""", protectionLevelNone
        )
        add(
            Manifest.permission.RECEIVE_BOOT_COMPLETED,
            1,
            null,
            """Allows an application to receive the Intent.ACTION_BOOT_COMPLETED that is broadcast after the system finishes booting. If you don't request this permission, you will not receive the broadcast at that time. Though holding this permission does not have any security implications, it can have a negative impact on the user experience by increasing the amount of time it takes the system to start and allowing applications to have themselves running without the user being aware of them. As such, you must explicitly declare your use of this facility to make that visible to the user. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.RECEIVE_MMS,
            1,
            null,
            """Allows an application to monitor incoming MMS messages. """,
            protectionLevelDangerous
        )
        add(
            Manifest.permission.RECEIVE_SMS,
            1,
            null,
            """Allows an application to receive SMS messages. """,
            protectionLevelDangerous
        )
        add(
            Manifest.permission.RECEIVE_WAP_PUSH,
            1,
            null,
            """Allows an application to receive WAP push messages. """,
            protectionLevelDangerous
        )
        add(
            Manifest.permission.RECORD_AUDIO,
            1,
            null,
            """Allows an application to record audio. """,
            protectionLevelDangerous
        )
        add(
            Manifest.permission.REORDER_TASKS,
            1,
            null,
            """Allows an application to change the Z-order of tasks. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.REQUEST_COMPANION_RUN_IN_BACKGROUND,
            26,
            null,
            """Allows a companion app to run in the background. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.REQUEST_COMPANION_USE_DATA_IN_BACKGROUND,
            26,
            null,
            """Allows a companion app to use data in the background. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.REQUEST_DELETE_PACKAGES,
            26,
            null,
            """Allows an application to request deleting packages. Apps targeting APIs Build.VERSION_CODES.P or greater must hold this permission in order to use Intent.ACTION_UNINSTALL_PACKAGE or PackageInstaller.uninstall(VersionedPackage, IntentSender). """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.REQUEST_IGNORE_BATTERY_OPTIMIZATIONS,
            23,
            null,
            """Permission an application must hold in order to use Settings.ACTION_REQUEST_IGNORE_BATTERY_OPTIMIZATIONS. This is a normal permission: an app requesting it will always be granted the permission, without the user needing to approve or see it.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.REQUEST_INSTALL_PACKAGES,
            23,
            null,
            """Allows an application to request installing packages. Apps targeting APIs greater than 25 must hold this permission in order to use Intent.ACTION_INSTALL_PACKAGE. """,
            protectionLevelSignature
        )
        add(
            // Android Q SDK not released
//            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q)
//                Manifest.permission.REQUEST_PASSWORD_COMPLEXITY
//            else
                "android.permission.REQUEST_PASSWORD_COMPLEXITY",
            29,
            null,
            """Allows an application to request the screen lock complexity and prompt users to update the screen lock to a certain complexity level. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.RESTART_PACKAGES, 1, 15, """This constant was deprecated in API level 15.
The ActivityManager.restartPackage(String) API is no longer supported. """, protectionLevelNone
        )
        add(
            Manifest.permission.SEND_RESPOND_VIA_MESSAGE,
            18,
            null,
            """Allows an application (Phone) to send a request to other applications to handle the respond-via-message action during incoming calls.

Not for use by third-party applications.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.SEND_SMS,
            1,
            null,
            """Allows an application to send SMS messages. """,
            protectionLevelDangerous
        )
        add(
            Manifest.permission.SET_ALARM,
            9,
            null,
            """Allows an application to broadcast an Intent to set an alarm for the user. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.SET_ALWAYS_FINISH,
            1,
            null,
            """Allows an application to control whether activities are immediately finished when put in the background.

Not for use by third-party applications.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.SET_ANIMATION_SCALE, 1, null, """Modify the global animation scaling factor.

Not for use by third-party applications.""", protectionLevelNone
        )
        add(
            Manifest.permission.SET_DEBUG_APP, 1, null, """Configure an application for debugging.

Not for use by third-party applications.""", protectionLevelNone
        )
        add(
            Manifest.permission.SET_PREFERRED_APPLICATIONS, 1, 15, """This constant was deprecated in API level 15.
No longer useful, see PackageManager.addPackageToPreferred(String) for details. """, protectionLevelNone
        )
        add(
            Manifest.permission.SET_PROCESS_LIMIT,
            1,
            null,
            """Allows an application to set the maximum number of (not needed) application processes that can be running.

Not for use by third-party applications. """,
            protectionLevelNone
        )
        add(
            Manifest.permission.SET_TIME, 8, null, """Allows applications to set the system time.

Not for use by third-party applications.""", protectionLevelNone
        )
        add(
            Manifest.permission.SET_TIME_ZONE, 1, null, """Allows applications to set the system time zone.

Not for use by third-party applications.""", protectionLevelNone
        )
        add(
            Manifest.permission.SET_WALLPAPER,
            1,
            null,
            """Allows applications to set the wallpaper. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.SET_WALLPAPER_HINTS,
            1,
            null,
            """Allows applications to set the wallpaper hints. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.SIGNAL_PERSISTENT_PROCESSES,
            1,
            null,
            """Allow an application to request that a signal be sent to all persistent processes.

Not for use by third-party applications.""",
            protectionLevelNone
        )
        add(
            // Android Q SDK not released
//            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q)
//                Manifest.permission.SMS_FINANCIAL_TRANSACTIONS
//            else
                "android.permission.SMS_FINANCIAL_TRANSACTIONS",
            29,
            null,
            """Allows financial apps to read filtered sms messages.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.STATUS_BAR,
            1,
            null,
            """Allows an application to open, close, or disable the status bar and its icons.

Not for use by third-party applications.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.SYSTEM_ALERT_WINDOW,
            1,
            null,
            """Allows an app to create windows using the type WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY, shown on top of all other apps. Very few apps should use this permission; these windows are intended for system-level interaction with the user.

Note: If the app targets API level 23 or higher, the app user must explicitly grant this permission to the app through a permission management screen. The app requests the user's approval by sending an intent with action Settings.ACTION_MANAGE_OVERLAY_PERMISSION. The app can check whether it has this authorization by calling Settings.canDrawOverlays(). """,
            protectionLevelSignature
        )
        add(
            Manifest.permission.TRANSMIT_IR,
            19,
            null,
            """Allows using the device's IR transmitter, if available. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.UNINSTALL_SHORTCUT, 19, null, """Don't use this permission in your app.
This permission is no longer supported.""", protectionLevelNone
        )
        add(
            Manifest.permission.UPDATE_DEVICE_STATS, 3, null, """Allows an application to update device statistics.

Not for use by third-party applications.""", protectionLevelNone
        )
//        add(
//            Manifest.permission.USE_BIOMETRIC,
//            28,
//            null,
//            """Allows an app to use device supported biometric modalities. """,
//            protectionLevelNormal
//        )
        add(
            Manifest.permission.USE_FINGERPRINT, 23, 28, """ This constant was deprecated in API level 28.
Applications should request USE_BIOMETRIC instead

Allows an app to use fingerprint hardware. """, protectionLevelNormal
        )
        add(
            // Android Q SDK not released
//            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q)
//                Manifest.permission.USE_FULL_SCREEN_INTENT
//            else
                "android.permission.USE_FULL_SCREEN_INTENT",
            29,
            null,
            """Required for apps targeting Build.VERSION_CODES.Q that want to use notification full screen intents.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.USE_SIP,
            9,
            null,
            """Allows an application to use SIP service. """,
            protectionLevelDangerous
        )
        add(Manifest.permission.VIBRATE, 1, null, """Allows access to the vibrator. """, protectionLevelNormal)
        add(
            Manifest.permission.WAKE_LOCK,
            1,
            null,
            """Allows using PowerManager WakeLocks to keep processor from sleeping or screen from dimming. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.WRITE_APN_SETTINGS,
            1,
            null,
            """Allows applications to write the apn settings and read sensitive fields of an existing apn settings like user and password.

Not for use by third-party applications.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.WRITE_CALENDAR,
            1,
            null,
            """Allows an application to write the user's calendar data. """,
            protectionLevelDangerous
        )
        add(
            Manifest.permission.WRITE_CALL_LOG,
            16,
            null,
            """Allows an application to write (but not read) the user's call log data.

Note: If your app uses the WRITE_CONTACTS permission and both your minSdkVersion and targetSdkVersion values are set to 15 or lower, the system implicitly grants your app this permission. If you don't need this permission, be sure your targetSdkVersion is 16 or higher.""",
            protectionLevelDangerous
        )
        add(
            Manifest.permission.WRITE_CONTACTS,
            1,
            null,
            """Allows an application to write the user's contacts data. """,
            protectionLevelDangerous
        )
        add(
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            4,
            null,
            """Allows an application to write to external storage.

Note: If both your minSdkVersion and targetSdkVersion values are set to 3 or lower, the system implicitly grants your app this permission. If you don't need this permission, be sure your targetSdkVersion is 4 or higher.

Starting in API level 19, this permission is not required to read/write files in your application-specific directories returned by Context.getExternalFilesDir(String) and Context.getExternalCacheDir().""",
            protectionLevelNone
        )
        add(
            Manifest.permission.WRITE_GSERVICES, 1, null, """Allows an application to modify the Google service map.

Not for use by third-party applications.""", protectionLevelNone
        )
        add(
            Manifest.permission.WRITE_SECURE_SETTINGS,
            3,
            null,
            """Allows an application to read or write the secure system settings.

Not for use by third-party applications.""",
            protectionLevelNone
        )
        add(
            Manifest.permission.WRITE_SETTINGS,
            1,
            null,
            """Allows an application to read or write the system settings.

Note: If the app targets API level 23 or higher, the app user must explicitly grant this permission to the app through a permission management screen. The app requests the user's approval by sending an intent with action Settings.ACTION_MANAGE_WRITE_SETTINGS. The app can check whether it has this authorization by calling Settings.System.canWrite(). """,
            protectionLevelSignature
        )
        add(
            Manifest.permission.WRITE_SYNC_SETTINGS,
            1,
            null,
            """Allows applications to write the sync settings. """,
            protectionLevelNormal
        )
        add(
            Manifest.permission.WRITE_VOICEMAIL,
            21,
            null,
            """Allows an application to modify and remove existing voicemails in the system. """,
            protectionLevelSignature,
            protectionLevelPrivileged
        )
    }
}