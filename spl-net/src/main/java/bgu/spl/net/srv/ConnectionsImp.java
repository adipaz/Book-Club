package bgu.spl.net.srv;

import javafx.util.Pair;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

public class ConnectionsImp<T> implements Connections<T> {

    public Map<Integer, ConnectionHandler> connectionHandlerPerUser;

    public ConnectionsImp()
    {
        connectionHandlerPerUser = new ConcurrentHashMap<>();
    }

    @Override
    public boolean send(int connectionId, T msg) {
        if(connectionHandlerPerUser.containsKey(connectionId))
        {

                connectionHandlerPerUser.get(connectionId).send(msg);
                return true;

        }
        return false;
    }

    @Override
    public void send(String channel, T msg) {
        Queue<Integer> usersIdForGenre = BookClub.getInstance().getUsersQueuePerGenre(channel);
        if(usersIdForGenre != null) //there are users that are subscribed to this channel
        {
            synchronized (usersIdForGenre) //to prevent: one thread going over the thread, other thread pulled out user, and the first thread will face a null although it wasn't null before
            {
                for (Integer id: usersIdForGenre) {
                    int subscriptionIdPerUserId = BookClub.getInstance().getGenreId(channel,id);
                    msg = (T) msg.toString().replaceFirst("%%%", String.valueOf(subscriptionIdPerUserId));
                    send(id, msg);
                }
            }
        }
    }

    @Override
    public void disconnect(int connectionId) {
        UsersHandler.getInstance().removeActiveUser(connectionId);
        BookClub.getInstance().removeUserFromHisTopics(connectionId);
        //no need to close - the client will close it from his side
    }

    public void addConnectionHandlerToUser(ConnectionHandler handler, int userId)
    {
        connectionHandlerPerUser.put(userId, handler);
    }
}
