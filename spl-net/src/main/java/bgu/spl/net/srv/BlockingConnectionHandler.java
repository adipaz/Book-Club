package bgu.spl.net.srv;

import bgu.spl.net.api.MessageEncoderDecoder;
import bgu.spl.net.api.MessageEncoderDecoderImp;
import bgu.spl.net.api.MessagingProtocol;
import bgu.spl.net.api.StompMessagingProtocolImp;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.util.concurrent.atomic.AtomicInteger;

public class BlockingConnectionHandler<T> implements Runnable, ConnectionHandler<T> {

    private final StompMessagingProtocolImp protocol;
    private final MessageEncoderDecoderImp encdec;
    private final Socket sock;
    private BufferedInputStream in;
    private BufferedOutputStream out;
    private volatile boolean connected = true;
    private volatile Connections connections;
    private static AtomicInteger userIdCounter;

    public BlockingConnectionHandler(Socket sock, MessageEncoderDecoderImp reader, StompMessagingProtocolImp protocol, Connections connections) {
        this.sock = sock;
        this.encdec = reader;
        this.protocol = protocol;
        this.connections = connections;
        if (userIdCounter == null)
            userIdCounter = new AtomicInteger(0);
    }

    @Override
    public void run() {
        try (Socket sock = this.sock) { //just for automatic closing
            int read;

            in = new BufferedInputStream(sock.getInputStream());
            out = new BufferedOutputStream(sock.getOutputStream());

            boolean hasConnected = false;
            while (!protocol.shouldTerminate() && connected && (read = in.read()) >= 0) {
                String nextMessage = encdec.decodeNextByte((byte) read);
                if (nextMessage != null) {
                    if (!hasConnected & nextMessage.contains("CONNECT"))
                    {
                        hasConnected = true;
                        protocol.start(userIdCounter.incrementAndGet(), connections);
                        ((ConnectionsImp) connections).addConnectionHandlerToUser(this, userIdCounter.get());
                    }
                    if(hasConnected) //the first command must be CONNECT
                        protocol.process(nextMessage);
                }
            }

        } catch (IOException ex) { }

    }

    @Override
    public void close() throws IOException {
        connected = false;
        sock.close();
    }

    @Override
    public void send(T msg) {
        try {
            out.write(encdec.encode(msg.toString()));
            out.flush();
        } catch (IOException e) { }
    }
}
