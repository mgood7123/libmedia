package libclient_service;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.os.Messenger;

import androidx.annotation.Nullable;

import java.util.ArrayList;
import java.util.concurrent.atomic.AtomicBoolean;

import static libclient_service.LibService_MessageCodes.MSG_REGISTERED_CLIENT;
import static libclient_service.LibService_MessageCodes.MSG_REGISTER_CLIENT;
import static libclient_service.LibService_MessageCodes.MSG_REGISTRATION_CONFIRMED;
import static libclient_service.LibService_MessageCodes.MSG_UNREGISTERED_CLIENT;
import static libclient_service.LibService_MessageCodes.MSG_UNREGISTER_CLIENT;

public abstract class LibService_Service_Component extends Service {

    Runnable onCreateCallback = null;

    public LibService_LogUtils log = new LibService_LogUtils("LibService - Service - Component");

    /** Keeps track of all current registered clients. */
    public ArrayList<Messenger> mClients = new ArrayList<Messenger>();

    public final LibService_Messenger messenger = new LibService_Messenger("Service");

    public LibService_Service_Connection manager = null;

    public AtomicBoolean onServiceConnectedCallbackCalled = new AtomicBoolean(false);

    /**
     * This service is only bound from inside the same process and never uses IPC.
     */
    public class Local extends Binder {
        public final LibService_Service_Component service = LibService_Service_Component.this;
    }

    public abstract void onMessengerAddResponses();

    public abstract void onMessengerBindLocal();
    public abstract void onMessengerBindRemote();

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        log.logMethodName_Info();
        if (intent.hasExtra("BINDING_TYPE")) {
            if (intent.getStringExtra("BINDING_TYPE").contentEquals("BINDING_LOCAL")) {
                onMessengerBindLocal();
                return new Local();
            }
        }
        messenger
            .addResponse(LibService_Messenger.PING, message -> {
                log.log_Info("sending pong");
                messenger.sendMessageToServer(message, LibService_Messenger.PONG);
                log.log_Info("sent pong");
            })
            .addResponse(MSG_REGISTER_CLIENT, (message) -> {
                // TODO: move this into libService
                // wait for the activity service to start and bind with its activity
                if (!onServiceConnectedCallbackCalled.get()) {
                    log.log_Error("waiting for onServiceConnectedCallbackCalled");
                    InfiniteLoop infiniteLoop = new InfiniteLoop();
                    infiniteLoop.setSleepTimeInMicroseconds(500);
                    infiniteLoop.loop(() -> !onServiceConnectedCallbackCalled.get());
                    log.log_Error("onServiceConnectedCallbackCalled has been set");
                }
                mClients.add(message.replyTo);
                log.log_Info("SERVER: registered client");
                log.log_Info("SERVER: informing client of registration");
                messenger.sendMessageToServer(message, MSG_REGISTERED_CLIENT);
            })
            .addResponse(MSG_REGISTRATION_CONFIRMED, (message) -> {
                // TODO: move this into libService
                log.log_Info("SERVER: informed client of registration");
            })
            .addResponse(MSG_UNREGISTER_CLIENT, (message) -> {
                // TODO: move this into libService
                log.log_Info("SERVER: unregistering client");
                messenger.sendMessageToServer(message, MSG_UNREGISTERED_CLIENT);
                mClients.remove(message.replyTo);
                log.log_Info("SERVER: unregistered client");
            });
        onMessengerAddResponses();
        onMessengerBindRemote();
        messenger.start();
        return messenger.getBinder();
    }

    @Override
    public void onCreate() {
        if (onCreateCallback != null) onCreateCallback.run();
    }
}
