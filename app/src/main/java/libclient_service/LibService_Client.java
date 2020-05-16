package libclient_service;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Intent;
import android.os.IBinder;

import static libclient_service.LibService_MessageCodes.MSG_REGISTERED_CLIENT;
import static libclient_service.LibService_MessageCodes.MSG_REGISTER_CLIENT;
import static libclient_service.LibService_MessageCodes.MSG_REGISTRATION_CONFIRMED;
import static libclient_service.LibService_MessageCodes.MSG_UNREGISTERED_CLIENT;

/** this class defines a set of API's used for creating a Client that can connect to a Service */

public abstract class LibService_Client {

    public LibService_LogUtils log = new LibService_LogUtils("LibService - Client ");

    public final LibService_Messenger messenger = new LibService_Messenger("Client");

    public abstract void onMessengerAddResponses();

    public abstract void onServiceConnectedCallback(IBinder boundService);
    public abstract void onServiceDisconnectedCallback(ComponentName name);

    final LibService_Service_Connection connection = new LibService_Service_Connection() {
        LibService_Client client = LibService_Client.this;

        @Override
        public void onServiceConnectedCallback(IBinder boundService) {
            client.messenger
                .addResponse(LibService_Messenger.PONG)
                .addResponse(MSG_REGISTERED_CLIENT, (message) -> {
                    // TODO: move this into libService
                    client.messenger.log.log_Info("registered");
                    client.messenger.sendMessageToServer(message, MSG_REGISTRATION_CONFIRMED);
                    client.messenger.log.log_Info("sent message to server");
                })
                .addResponse(MSG_UNREGISTERED_CLIENT, (message) -> {
                    // TODO: move this into libService
                    client.messenger.log.log_Info("unregistered");
                })
                .bind(boundService)
                .start();
            client.onMessengerAddResponses();
            client.log.log_Info("registering");
            client.messenger.sendMessageToServer(MSG_REGISTER_CLIENT);
            client.log.log_Info("registered");
            client.onServiceConnectedCallback(boundService);
        }

        @Override
        public void onServiceDisconnectedCallback(ComponentName name) {
            client.onServiceDisconnectedCallback(name);
        }
    };

    public void connectToService(Activity activity, String packageName, String serviceClassName) {
        Intent serviceIntent = new Intent();
        serviceIntent.setPackage(packageName);
        serviceIntent.setClassName(packageName, serviceClassName);

        // Establish a connection with the service.  We use an explicit
        // class name because there is no reason to be able to let other
        // applications replace our component.
        if (!activity.bindService(serviceIntent, connection, 0)) {
            throw new RuntimeException("bindService() failed");
        }
    }
}
