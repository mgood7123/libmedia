/*
*************************************************************************
Alpine Term - a VM-based terminal emulator.
Copyright (C) 2019-2020  Leonid Plyushch <leonid.plyushch@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*************************************************************************
*/
package alpine.term;

import android.content.Context;

/**
 * Application build-time configuration entries.
 */
@SuppressWarnings("WeakerAccess")
public class Config {

    LogUtils logUtils = new LogUtils("Config");

    /**
     * Name of CD-ROM image file.
     * Must be a name of file located in assets directory.
     */
    public static final String CDROM_IMAGE_NAME = "alpine-linux-cdrom.iso";

    /**
     * Name of HDD image file.
     * Must be a name of file located in assets directory.
     */
    public static final String HDD_IMAGE_NAME = "alpine-linux-hdd.qcow2";

    /**
     * Name of zip archive with QEMU firmware & keymap files + ca certificates
     * bundle used by libcurl.
     * Must be a name of file located in assets directory.
     */
    public static final String QEMU_DATA_PACKAGE = "platform-independent-data.bin";

    /**
     * Upstream DNS server used by QEMU DNS resolver.
     */
    public static final String QEMU_UPSTREAM_DNS = "8.8.8.8";

    /**
     * A tag used for general logging.
     */
    public static final String APP_LOG_TAG = "alpine-term:app";

    /**
     * A tag used for input (ime) logging.
     */
    public static final String INPUT_LOG_TAG = "alpine-term:input";

    /**
     * A tag used for installer logging.
     */
    public static final String INSTALLER_LOG_TAG = "alpine-term:installer";

    /**
     * A tag used for wakelock logging.
     */
    public static final String WAKELOCK_LOG_TAG = "alpine-term:wakelock";

    /**
     * Returns path to runtime environment directory.
     */
    public static String getDataDirectory(final Context context) {
        return context.getFilesDir().getAbsolutePath();
    }

    /**
     * Returns path to directory where temporary files can be placed.
     */
    public static String getTemporaryDirectory(final Context context) {
        return context.getCacheDir().getAbsolutePath();
    }
}
