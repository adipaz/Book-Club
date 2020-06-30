package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.MessageEncoderDecoderImp;
import bgu.spl.net.api.StompMessagingProtocolImp;
import bgu.spl.net.srv.BaseServer;
import bgu.spl.net.srv.BlockingConnectionHandler;
import bgu.spl.net.srv.Reactor;
import bgu.spl.net.srv.Server;

import java.util.function.Supplier;

public class StompServer {

    public static void main(String[] args) {
        String portString = args[0];
        String serverType = args[1];
        Supplier<StompMessagingProtocolImp> protocolFactory = StompMessagingProtocolImp::new;
        Supplier<MessageEncoderDecoderImp> encdecFactory = MessageEncoderDecoderImp::new;
        int port = Integer.parseInt(portString);
        if(serverType.equals("tpc")) //thread per client
        {
            Server.threadPerClient(port, protocolFactory, encdecFactory).serve();
        }
        else //Reactor
        {
           Server.reactor(5,port,protocolFactory,encdecFactory).serve();
        }
    }


}
