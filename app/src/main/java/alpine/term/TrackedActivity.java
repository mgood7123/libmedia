package alpine.term;

import android.content.pm.ActivityInfo;
import android.os.Parcel;
import android.os.ParcelFileDescriptor;
import android.os.Parcelable;

import java.io.FileDescriptor;
import java.io.IOException;
import java.lang.reflect.Field;

/**
 * generated via <a href="http://www.parcelabler.com/">http://www.parcelabler.com/</a>
 */
@SuppressWarnings("WeakerAccess")
public class TrackedActivity implements Parcelable {

    static LogUtils logUtils = new LogUtils("Tracked Activity");

    TrackedActivity() {}
    public ActivityInfo activityInfo = null;
    public String packageName = "<INVALID>";
    public String label = "";
    public int pid = 0;
    public String pidAsString;
    public ParcelFileDescriptor pseudoTerminalMaster = null;
    public ParcelFileDescriptor pseudoTerminalSlave = null;
    public String description;

    /**
     * this wraps a raw native fd into a java {@link FileDescriptor}
     * @param fd a raw native fd
     * @return a valid FileDescriptor, otherwise null
     */
    public static FileDescriptor wrapFileDescriptor(int fd) {
        FileDescriptor result = new FileDescriptor();
        try {
            Field descriptorField;
            try {
                descriptorField = FileDescriptor.class.getDeclaredField("descriptor");
            } catch (NoSuchFieldException e) {
                // For desktop java:
                descriptorField = FileDescriptor.class.getDeclaredField("fd");
            }
            boolean originalAccessibility = descriptorField.isAccessible();
            descriptorField.setAccessible(true);
            descriptorField.set(result, fd);
            descriptorField.setAccessible(originalAccessibility);
        } catch (NoSuchFieldException | IllegalAccessException | IllegalArgumentException e) {
            logUtils.log_Error("error accessing FileDescriptor#descriptor private field", e);
            return null;
        }
        return result;
    }

    /**
     * this unwraps a java {@link FileDescriptor} into a raw native fd
     * @param fileDescriptor a java {@link FileDescriptor}
     * @return a raw native fd
     */
    public static int unwrapFileDescriptor(FileDescriptor fileDescriptor) {
        int fd = -1;
        try {
            Field descriptorField;
            try {
                descriptorField = FileDescriptor.class.getDeclaredField("descriptor");
            } catch (NoSuchFieldException e) {
                // For desktop java:
                descriptorField = FileDescriptor.class.getDeclaredField("fd");
            }
            boolean originalAccessibility = descriptorField.isAccessible();
            descriptorField.setAccessible(true);
            fd = descriptorField.getInt(fileDescriptor);
            descriptorField.setAccessible(originalAccessibility);
        } catch (NoSuchFieldException | IllegalAccessException | IllegalArgumentException e) {
            logUtils.log_Error("error accessing FileDescriptor#descriptor private field", e);
            return -1;
        }
        return fd;
    }

    /**
     * stores a PseudoTerminal returned by {@link TerminalClientAPI#createPseudoTerminal()},
     * this can be obtained via {@link #getPseudoTerminal()}
     * @return true if the PseudoTerminal was stored successfully, otherwise false
     */
    public boolean storePseudoTerminal(int[] fd) {
        // https://github.com/hacking-android/frameworks/blob/943f0b4d46f72532a419fb6171e40d1c93984c8e/devices/google/Pixel%202/29/QPP6.190730.005/src/framework/android/net/IpSecUdpEncapResponse.java#L49
        try {
            FileDescriptor fileDescriptor0 = wrapFileDescriptor(fd[0]);
            FileDescriptor fileDescriptor1 = wrapFileDescriptor(fd[1]);
            if (fileDescriptor0 == null || fileDescriptor1 == null) return false;
            pseudoTerminalMaster = ParcelFileDescriptor.dup(fileDescriptor0);
            pseudoTerminalSlave = ParcelFileDescriptor.dup(fileDescriptor1);
            return true;
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
    }

    /**
     * obtains the fd stored from {@link #storePseudoTerminal(int[])}
     * @return an fd, this may be null if an error has occurred
     */
    public int[] getPseudoTerminal() {
        int[] fds = new int[2];
        fds[0] = unwrapFileDescriptor(pseudoTerminalMaster.getFileDescriptor());
        fds[1] = unwrapFileDescriptor(pseudoTerminalSlave.getFileDescriptor());
        return fds[0] == -1 || fds[1] == -1 ? null : fds;
    }

    protected TrackedActivity(Parcel in) {
        activityInfo = (ActivityInfo) in.readValue(ActivityInfo.class.getClassLoader());
        packageName = in.readString();
        label = in.readString();
        pid = in.readInt();
        pidAsString = in.readString();
        pseudoTerminalMaster = in.readParcelable(ParcelFileDescriptor.class.getClassLoader());
        pseudoTerminalSlave = in.readParcelable(ParcelFileDescriptor.class.getClassLoader());
        description = in.readString();
    }

    @Override
    public int describeContents() {
        return CONTENTS_FILE_DESCRIPTOR;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeValue(activityInfo);
        dest.writeString(packageName);
        dest.writeString(label);
        dest.writeInt(pid);
        dest.writeString(pidAsString);
        dest.writeParcelable(this.pseudoTerminalMaster, 1);
        dest.writeParcelable(this.pseudoTerminalSlave, 1);
        dest.writeString(description);
    }

    @SuppressWarnings("unused")
    public static final Creator<TrackedActivity> CREATOR = new Creator<TrackedActivity>() {
        @Override
        public TrackedActivity createFromParcel(Parcel in) {
            return new TrackedActivity(in);
        }

        @Override
        public TrackedActivity[] newArray(int size) {
            return new TrackedActivity[size];
        }
    };
}
