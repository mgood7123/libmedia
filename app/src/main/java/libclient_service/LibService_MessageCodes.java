package libclient_service;

public final class LibService_MessageCodes {

    /**
     * Command to the service to register a client, receiving callbacks
     * from the service.  The Message's replyTo field must be a Messenger of
     * the client where callbacks should be sent.
     */
    static final int MSG_REGISTER_CLIENT = 1;
    static final int MSG_REGISTRATION_CONFIRMED = 12345;
    static final int MSG_REGISTERED_CLIENT = 3;

    /**
     * Command to the service to unregister a client, ot stop receiving callbacks
     * from the service.  The Message's replyTo field must be a Messenger of
     * the client as previously given with MSG_REGISTER_CLIENT.
     */

    static final int MSG_UNREGISTER_CLIENT = 4;
    static final int MSG_UNREGISTERED_CLIENT = 5;

}
