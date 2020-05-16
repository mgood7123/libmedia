package libclient_service;

import android.app.Activity;
import android.app.Service;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;

import java.util.ArrayList;

/** this class defines a set of API's used for creating a Service that a Client can connect to */

public abstract class LibService_Service_Connection implements ServiceConnection {

    public LibService_LogUtils log = new LibService_LogUtils("LibService - Service Connection");

    public final LibService_Messenger messenger = new LibService_Messenger("Service Connection");

    ArrayList<Class<? extends LibService_Service_Component>> components = new ArrayList<>();

    Activity activity = null;

    Context context = null;

    public LibService_Service_Component service = null;

    public final void onServiceConnectedCallback() {
        onServiceConnectedCallback(null);
    };
    public abstract void onServiceConnectedCallback(IBinder boundService);

    @Override
    public void onServiceConnected(ComponentName componentName, IBinder boundService) {
        log.logMethodName_Info();

        // can probs be fixed by connection/binding re-ordering

        boolean isLocal = boundService instanceof LibService_Service_Component.Local;

        if (isLocal) {
            LibService_Service_Component.Local local =
                (LibService_Service_Component.Local) boundService;
            service = local.service;
            context = service.getApplicationContext();
            service.manager = this;
            log.log_Info("onServiceConnected: binded to local service");
            log.errorAndThrowIfNull(service);
            onServiceConnectedCallback(boundService);
            service.onServiceConnectedCallbackCalled.set(true);
        } else {
            log.log_Info("onServiceConnected: binded to remote service");
            onServiceConnectedCallback(boundService);
        }
    }

    /**
     * Called when a connection to the Service has been lost.  This typically
     * happens when the process hosting the service has crashed or been killed.
     * This does <em>not</em> remove the ServiceConnection itself -- this
     * binding to the service will remain active, and you will receive a call
     * to {@link #onServiceConnected} when the Service is next running.
     *
     * @param name The concrete component name of the service whose
     *             connection has been lost.
     */
    @Override
    public void onServiceDisconnected(ComponentName name) {
        log.logMethodName_Info();
        onServiceDisconnectedCallback(name);
    }

    public abstract void onServiceDisconnectedCallback(ComponentName name);

    /**
     * Called when the binding to this connection is dead.  This means the
     * interface will never receive another connection.  The application will
     * need to unbind and rebind the connection to activate it again.  This may
     * happen, for example, if the application hosting the service it is bound to
     * has been updated.
     *
     * @param name The concrete component name of the service whose
     *             connection is dead.
     */
    @Override
    public void onBindingDied(ComponentName name) {
        log.logMethodName_Info();
    }

    /**
     * Called when the service being bound has returned {@code null} from its
     * {@link Service#onBind(Intent) onBind()} method.  This indicates
     * that the attempting service binding represented by this ServiceConnection
     * will never become usable.
     *
     * <p class="note">The app which requested the binding must still call
     * {@link Context#unbindService(ServiceConnection)} to release the tracking
     * resources associated with this ServiceConnection even if this callback was
     * invoked following {@link Context#bindService Context.bindService() bindService()}.
     *
     * @param name The concrete component name of the service whose binding
     *             has been rejected by the Service implementation.
     */
    @Override
    public void onNullBinding(ComponentName name) {
        log.logMethodName_Info();
    }
}
