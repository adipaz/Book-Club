package bgu.spl.net.api;

import bgu.spl.net.srv.*;
import javafx.util.Pair;

import java.util.Arrays;
import java.util.concurrent.atomic.AtomicInteger;


public class StompMessagingProtocolImp implements StompMessagingProtocol {
    Connections<String> connections;
    int connectionId;
    private static AtomicInteger msgID;
    private boolean terminate;

    @Override
    public void start(int connectionId, Connections<String> connections) {
        this.connectionId=connectionId;
        this.connections=connections;
        if(msgID == null) {
            msgID = new AtomicInteger(0);
        }
        terminate = false;
    }

    @Override
    public void process(String message) {
        int msgId =  msgID.incrementAndGet();
        String[] commands = message.split("[\\r\\n]+");
        if(commands[0].equals("CONNECT")){
            handleConnectFrame(message, commands, msgId);
        }
        else if(commands[0].toUpperCase().equals("SUBSCRIBE")){
            handleSubscribeFrame(commands, msgId);
        }
        else if(commands[0].toUpperCase().equals("SEND")){
            handleSendFrame(commands, msgId);
        }
        else if(commands[0].toUpperCase().equals("DISCONNECT")){
            handleDisconnectFrame(commands, msgId);
        }
        else if(commands[0].equals("UNSUBSCRIBE")){
            handleUnsubscribeFrame(commands, msgId);
        }
    }

    @Override
    public boolean shouldTerminate() {
        return terminate;
    }

    //supporting the possibility of more unneeded rows in the header and when they are not in a specific order
    private String[] parseLoginFrame(String[] commands)
    {
        String[] loginCommand = new String[3];
        for (String element : commands) {
            if (element.contains("accept-version"))
            {
                loginCommand[0] = element.split(":")[1];
            }
            else if(element.contains("login"))
            {
                loginCommand[1] = element.split(":")[1];
            }
            else if(element.contains("passcode"))
            {
                loginCommand[2] = element.split(":")[1];
            }
        }
        return loginCommand;
    }

    private void handleConnectFrame(String originalMsg, String[] commands, int msgId)
    {
        String[] loginCommands = parseLoginFrame(commands);
        String result = UsersHandler.getInstance().addNewUser(loginCommands[1],loginCommands[2], connectionId);
        if(result.equals("success"))
        {
            String successFrame = "CONNECTED\nversion:" + loginCommands[0] + "\n\n";
            connections.send(connectionId,successFrame);
        }
        else //there was some kind of error
        {
            String errorFrame = "ERROR\nreceipt-id: message-" + msgId + "\nmessage: CONNECT frame received\n\nThe message:\n---------\n"
                    + originalMsg + "\n---------\n" + result + "\n";
            connections.send(connectionId,errorFrame);
            terminate = true;
            connections.disconnect(connectionId);
        }
    }

    //supporting the possibility of more unneeded rows in the header and when they are not in a specific order
    private String[] parseSubscribeFrame(String[] commands)
    {
        String[] subCommand = new String[3];
        for (String element : commands) {
            if (element.contains("destination"))
            {
                subCommand[0] = element.split(":")[1];
            }
            else if(element.contains("id"))
            {
                subCommand[1] = element.split(":")[1];
            }
            else if(element.contains("receipt"))
            {
                subCommand[2] = element.split(":")[1];
            }
        }
        return subCommand;
    }

    private void handleSubscribeFrame(String[] commands, int msgId)
    {
        String[] subCommands = parseSubscribeFrame(commands);
        if(UsersHandler.getInstance().checkIfUserIsActive(connectionId)) //only if the user connected than act
        {
            BookClub.getInstance().subscribeUserToGenre(subCommands[0],connectionId, Integer.parseInt(subCommands[1]));
            String msg = "RECEIPT\nreceipt-id:" + subCommands[2] + "\n\n";
            connections.send(connectionId,msg);
        }
    }

    //supporting the possibility of more unneeded rows in the header and when they are not in a specific order
    private String[] parseDisconnectFrame(String[] commands)
    {
        String[] disCommand = new String[1];
        for (String element : commands) {
            if(element.contains("receipt"))
            {
                disCommand[0] = element.split(":")[1];
            }
        }
        return disCommand;
    }

    private void handleDisconnectFrame(String[] commands, int msgId)
    {
        String receiptIdStr = parseDisconnectFrame(commands)[0];
        if(UsersHandler.getInstance().checkIfUserIsActive(connectionId)) //only if the user connected than act
        {
            int receiptId = Integer.parseInt(receiptIdStr);
            String msg = "RECEIPT\nreceipt-id:" + receiptId + "\n\n";
            connections.send(connectionId,msg);
            connections.disconnect(connectionId);
        }
    }

    //supporting the possibility of more unneeded rows in the header and when they are not in a specific order
    private String[] parseUnsubscribeFrame(String[] commands)
    {
        String[] unsubCommand = new String[2];
        for (String element : commands) {
            if(element.contains("id"))
            {
                unsubCommand[0] = element.split(":")[1];
            }
            else if(element.contains("receipt"))
            {
                unsubCommand[1] = element.split(":")[1];
            }
        }
        return unsubCommand;
    }

    private void handleUnsubscribeFrame(String[] commands, int msgId)
    {
        String[] unsubCommand = parseUnsubscribeFrame(commands);
        if(UsersHandler.getInstance().checkIfUserIsActive(connectionId)) //only if the user connected than act
        {
            int genreId = Integer.parseInt(unsubCommand[0]);
            int receiptId = Integer.parseInt(unsubCommand[1]);
            BookClub.getInstance().removeUserFromTopic(genreId, connectionId);
            String msg = "RECEIPT\nreceipt-id:" + receiptId + "\n\n";
            connections.send(connectionId, msg);
        }
    }

    //supporting the possibility of more unneeded rows in the header and when they are not in a specific order
    private String[] parseSendFrame(String[] commands)
    {
        String[] sendCommand = new String[2];
        for(int i = 0; i < commands.length; i++)
        {
            if(commands[i].contains("destination"))
            {
                sendCommand[0] = commands[i].split(":")[1];
            }
            else if(i == commands.length - 1) //the body comes at the end - no matter the size and the content of the header
            {
                sendCommand[1] = commands[i];
            }
        }
        return sendCommand;
    }

    private void handleSendFrame(String[] commands, int msgId)
    {
        String[] sendCommand = parseSendFrame(commands);
        if(UsersHandler.getInstance().checkIfUserIsActive(connectionId)) //only if the user connected than act
        {
            String genreName = sendCommand[0];
            String bodyOfFrame = sendCommand[1];
            String msg = "MESSAGE\nsubscription:%%%\nMessage-id:" + msgId + "\ndestination:" + genreName + "\n\n" + bodyOfFrame + "\n";
            connections.send(genreName, msg);
        }
    }

}
