package libclient_service;

// SERVICE MAP - IMPORTANT - https://i.stack.imgur.com/VxM9P.png

import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;

import java.util.ArrayList;

/** messenger builder */

public class LibService_Messenger {

    public static final int PING = -5;
    public static final int PONG = -6;
    public LibService_LogUtils log;

    LibService_Messenger() {
        log = new LibService_LogUtils("LibService - LibMessenger - NO IDENTIFIER SUPPLIED");
    }

    LibService_Messenger(String identifier) {
         log = new LibService_LogUtils("LibService - LibMessenger - " + identifier);
    }

    /**
     * this object is used for synchronization
     */

    private final Object waitOnMe = new Object();

    public void waitForReply() {
        log.log_Info("waiting for reply");
        synchronized (waitOnMe) {
            try {
                waitOnMe.wait();
            } catch (InterruptedException e) {
                // we should have gotten our answer now.
            }
        }
        log.log_Info("replied");
    }

    public boolean sendMessageToServer(int what) {
        return sendMessageToServer(null, null, what, 0, 0, null, null);
    }

    private boolean sendMessageToServer(int what, int arg1) {
        return sendMessageToServer(null, null, what, arg1, 0, null, null);
    }

    private boolean sendMessageToServer(int what, int arg1, int arg2) {
        return sendMessageToServer(null, null, what, arg1, arg2, null, null);
    }

    public boolean sendMessageToServer(int what, Object obj) {
        return sendMessageToServer(null, null, what, 0, 0, obj, null);
    }

    private boolean sendMessageToServer(int what, int arg1, Object obj) {
        return sendMessageToServer(null, null, what, arg1, 0, obj, null);
    }

    private boolean sendMessageToServer(int what, int arg1, int arg2, Object obj) {
        return sendMessageToServer(null, null, what, arg1, arg2, obj, null);
    }

    public boolean sendMessageToServer(int what, Bundle bundle) {
        return sendMessageToServer(null, null, what, 0, 0, null, bundle);
    }

    private boolean sendMessageToServer(int what, int arg1, Bundle bundle) {
        return sendMessageToServer(null, null, what, arg1, 0, null, bundle);
    }

    private boolean sendMessageToServer(int what, int arg1, int arg2, Bundle bundle) {
        return sendMessageToServer(null, null, what, arg1, arg2, null, bundle);
    }

    public boolean sendMessageToServer(int what, Object obj, Bundle bundle) {
        return sendMessageToServer(null, null, what, 0, 0, obj, bundle);
    }

    private boolean sendMessageToServer(int what, int arg1, Object obj, Bundle bundle) {
        return sendMessageToServer(null, null, what, arg1, 0, obj, bundle);
    }

    private boolean sendMessageToServer(int what, int arg1, int arg2, Object obj, Bundle bundle) {
        return sendMessageToServer(null, null, what, arg1, arg2, obj, bundle);
    }

    public boolean sendMessageToServer(Message msg, int what) {
        return sendMessageToServer(msg, null, what, 0, 0, null, null);
    }

    private boolean sendMessageToServer(Message msg, int what, int arg1) {
        return sendMessageToServer(msg, null, what, arg1, 0, null, null);
    }

    private boolean sendMessageToServer(Message msg, int what, int arg1, int arg2) {
        return sendMessageToServer(msg, null, what, arg1, arg2, null, null);
    }

    public boolean sendMessageToServer(Message msg, int what, Object obj) {
        return sendMessageToServer(msg, null, what, 0, 0, obj, null);
    }

    private boolean sendMessageToServer(Message msg, int what, int arg1, Object obj) {
        return sendMessageToServer(msg, null, what, arg1, 0, obj, null);
    }

    private boolean sendMessageToServer(Message msg, int what, int arg1, int arg2, Object obj) {
        return sendMessageToServer(msg, null, what, arg1, arg2, obj, null);
    }

    public boolean sendMessageToServer(Message msg, int what, Bundle bundle) {
        return sendMessageToServer(msg, null, what, 0, 0, null, bundle);
    }

    private boolean sendMessageToServer(Message msg, int what, int arg1, Bundle bundle) {
        return sendMessageToServer(msg, null, what, arg1, 0, null, bundle);
    }

    private boolean sendMessageToServer(Message msg, int what, int arg1, int arg2, Bundle bundle) {
        return sendMessageToServer(msg, null, what, arg1, arg2, null, bundle);
    }

    public boolean sendMessageToServer(Message msg, int what, Object obj, Bundle bundle) {
        return sendMessageToServer(msg, null, what, 0, 0, obj, bundle);
    }

    private boolean sendMessageToServer(Message msg, int what, int arg1, Object obj, Bundle bundle) {
        return sendMessageToServer(msg, null, what, arg1, 0, obj, bundle);
    }

    private boolean sendMessageToServer(Message msg, int what, int arg1, int arg2, Object obj, Bundle bundle) {
        return sendMessageToServer(msg, null, what, arg1, arg2, obj, bundle);
    }

    public Messenger messengerToHandleMessages;
    public Messenger messengerToSendMessagesTo;
    final int DEFAULT_CODE = 9999;

    public void Notify() {
        // handled messages are handled in background thread
        // then notify about finished message.
        synchronized (waitOnMe) {
            waitOnMe.notifyAll();
        }
    }

    // DESIGN V1
    // each function runs in the main processing thread
    // each function is a RunnableArgument
    // if a function calls to client which calls back to
    // main processing thread then a *deadlock* will occur

    // this is due to the main processing thread
    // processing cmd A, during which, cmd A
    // requests client to process cmd B
    // which then sends response to server to
    // notify the server that cmd B has finished
    // however the server cannot be notified of this
    // as cmd A is waiting for cmd B to complete

    // DESIGN V2
    //  DEADLOCK RESOLUTION
    //      DEADLOCK
    //          this is due to the main processing thread
    //          processing cmd A, during which, cmd A
    //          requests client to process cmd B
    //          which then sends response to server to
    //          notify the server that cmd B has finished
    //          however the server cannot be notified of this
    //          as cmd A is waiting for cmd B to complete
    //
    //      HOW TO RESOLVE...
    //          INFO
    //              in a standard function, cmd A would invoke
    //              the instruction cmd B, which would run until
    //              completion, and return to the next instruction
    //              that is after the function call
    //          IMPORTANT
    //              cmd B request to the client, must be synchronized
    //              with the server
    //          RESOLUTION
    //              a call stack WILL NOT work as the client must be
    //              able to notify the server that the cmd requested by
    //              the server has been completed
    //
    //              an object may be updated if it is sent, updated, and obtained
    //
    //              if the command completions notification occur in a seperate thread as
    //              the command processes are taking place in, it is possible to inform
    //              the server of a command completion without blocking the main processing
    //              thread

    private static final int messageType_Message = 9997;
    private static final String messageType_Message_Bundle_Parcelable_Key = "Message";
    private static final int messageType_Notification = 9998;

    public boolean sendMessageToServer(Message message, Handler handler, int what, int arg1, int arg2, Object obj, Bundle bundle) {
        try {
            log.errorAndThrowIfNull(messengerToHandleMessages);
            Message msg = Message.obtain(handler, what, arg1, arg2, obj);
            if (bundle != null) msg.setData(bundle);
            msg.replyTo = messengerToHandleMessages;

            Message msgServer = Message.obtain(handler, messageType_Message);
            Bundle bund = new Bundle();
            bund.putParcelable(messageType_Message_Bundle_Parcelable_Key, msg);
            msgServer.setData(bund);

            if (message != null) message.replyTo.send(msgServer);
            else messengerToSendMessagesTo.send(msgServer);
            waitForReply();
            return true;
        } catch (RemoteException e) {
            // In this case the service has crashed before we could even
            // do anything with it
            return false;
        }
    }

    public boolean sendNotificationToServer(Message message) {
        try {
            Message msg = Message.obtain(handler, messageType_Notification);
            message.replyTo.send(msg);
            return true;
        } catch (RemoteException e) {
            // In this case the service has crashed before we could even
            // do anything with it
            return false;
        }
    }

    public void HandleMain(Message message) {
        boolean messageHandled = false;
        for (int i = 0, responsesSize = responses.size(); i < responsesSize; i++) {
            Response response = responses.get(i);
            if (message.what == response.what) {
                messageHandled = true;
                if (response.whatToExecute != null) response.whatToExecute.run(message);
                break;
            }
        }
        // TODO: handle unspecified response, by default we send and do not wait for a reply
        if (!messageHandled) {
            log.log_Warning("received an unknown response code: " + message.what);
        }
    }

    public static Thread mainThread = Looper.getMainLooper().getThread();

    public final HandlerThread handlerThread = new HandlerThread("LibMessenger");
    public Looper looper;
    public Handler handler;
    public final Handler.Callback callback = new Handler.Callback() {
        @Override
        public boolean handleMessage(Message message) {
            if (message == null) {
                log.log_Warning("message was null");
            } else {
                if (message.what == messageType_Notification) {
                    log.log_Info("received Notification");
                    new Thread() {
                        @Override
                        public void run() {
                            super.run();
                            synchronized (waitOnMe) {
                                waitOnMe.notifyAll();
                            }
                        }
                    }.start();
                } else if (message.what == messageType_Message) {
                    log.log_Info("received Message");
                    log.errorAndThrowIfNull(message);
                    Bundle bund = message.getData();
                    log.errorAndThrowIfNull(bund);
                    Message msg = bund.getParcelable(messageType_Message_Bundle_Parcelable_Key);
                    log.errorAndThrowIfNull(msg);
                    new Thread() {
                        @Override
                        public void run() {
                            super.run();
                            Looper.prepare();
                            HandleMain(msg);
                            log.log_Info("sending notification");
                            sendNotificationToServer(msg);
                            log.log_Info("sent Notification");
                            Looper.loop();
                        }
                    }.start();
                } else {
                    log.log_Info("received unknown message id: " + message.what);
                }
            }
            return true;
        }
    };

    class Response {
        int what = 0;
        RunnableArgument<Message> whatToExecute = null;
    }

    private ArrayList<Response> responses = new ArrayList<>();

    /*
server.defineResponseCodes(
    // service
    {
        // ...
    },
    // client
    {
        // ...
    }
};
     */

    public LibService_Messenger addResponse(int what) {
        Response response = new Response();
        response.what = what;
        responses.add(response);
        return this;
    }

    public LibService_Messenger addResponse(int what, RunnableArgument<Message> whatToExecute) {
        Response response = new Response();
        response.what = what;
        response.whatToExecute = whatToExecute;
        responses.add(response);
        return this;
    }

    public LibService_Messenger bind (IBinder serviceContainingMessenger) {
        log.errorAndThrowIfNull(serviceContainingMessenger);
        messengerToSendMessagesTo = new Messenger(serviceContainingMessenger);
        log.log_Info("binded to remote service");
        return this;
    }

    public void ping() {
        log.log_Info("sending ping");
        sendMessageToServer(PING);
        log.log_Info("sent ping");
    }

    public LibService_Messenger start() {
        if (handlerThread.getState() == Thread.State.NEW) {
            handlerThread.start();
            looper = handlerThread.getLooper();
            handler = new Handler(looper, callback);
            messengerToHandleMessages = new Messenger(handler);
            log.log_Info("started messenger");
        }
        return this;
    }

    public IBinder getBinder() {
        return messengerToHandleMessages.getBinder();
    }
}

