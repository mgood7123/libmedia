package libclient_service;

import android.app.Activity;
import android.content.Intent;

import java.util.ArrayList;

/** this class defines a set of API's used for creating a Service that a Client can connect to */

public class LibService_Service {

    private LibService_LogUtils log = new LibService_LogUtils("LibService - Service");

    ArrayList<LibService_Service_Connection> connections = new ArrayList<>();

    public void addService(Class<? extends LibService_Service_Component> service, int managerId) {
        LibService_Service_Connection connection = connections.get(managerId);
        connection.components.add(service);
    }

    public int addServiceManager(LibService_Service_Connection manager) {
        connections.add(manager);
        return connections.indexOf(manager);
    }

    public void bindLocal(Activity activity, int managerId) {
        LibService_Service_Connection manager = connections.get(managerId);
        manager.activity = activity;

        // start and bind each component in the component list
        for (int i = 0, componentsSize = manager.components.size(); i < componentsSize; i++) {
            Class<? extends LibService_Service_Component> service = manager.components.get(i);
            Intent serviceIntent = new Intent(activity.getApplicationContext(), service);
            serviceIntent.putExtra("BINDING_TYPE", "BINDING_LOCAL");

            // Start the service and make it run regardless of who is bound to it:
            activity.startService(serviceIntent);
            if (!activity.bindService(serviceIntent, manager, 0)) {
                throw new RuntimeException("bindService() failed");
            }
        }
    }
}
