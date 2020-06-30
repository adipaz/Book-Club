package bgu.spl.net.srv;

import javafx.util.Pair;

import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

public class BookClub {
    private Map<String,Queue<Integer>> usersPerGenre; //key - genre name, value - the users listed to the genre
    private Map<Integer, Queue<Pair<Integer,String>>> genreCodePerUser; //the key is the subscription id, the value is pair when the key is the user id and the value is the genre name

    private static class SingletonHolder {
        private static BookClub instance = new BookClub();
    }

    public static BookClub getInstance() {
        return SingletonHolder.instance;
    }

    private BookClub()
    {
        usersPerGenre = new ConcurrentHashMap<>();
        genreCodePerUser = new ConcurrentHashMap<>();
    }

    public void subscribeUserToGenre(String genre, int userId, int genreId) //if the user is already subscribed - do nothing
    {
        if(!usersPerGenre.containsKey(genre))
        {
            this.usersPerGenre.put(genre, new ConcurrentLinkedQueue<>());

        }
        if(genreCodePerUser.containsKey(genreId))
        {
            boolean found = false;
            for(Pair<Integer,String> pair: genreCodePerUser.get(genreId))
            {
                if(pair.getKey() == userId & pair.getValue().equals(genre))
                {
                    found = true;
                    break;
                }
            }
            if(!found)
            {
                this.genreCodePerUser.get(genreId).add(new Pair<>(userId,genre));
            }
        }
        else
        {
            this.genreCodePerUser.put(genreId, new ConcurrentLinkedQueue<>());
            this.genreCodePerUser.get(genreId).add(new Pair<>(userId,genre));
        }
        if(!usersPerGenre.get(genre).contains(userId))
        {
            this.usersPerGenre.get(genre).add(userId);
        }
    }

    public Queue<Integer> getUsersQueuePerGenre(String genre)
    {
        return this.usersPerGenre.get(genre);
    }

    public void removeUserFromHisTopics(int userId)
    {
        for (Queue<Integer> usersQueueOfTopic: usersPerGenre.values()) {
            usersQueueOfTopic.remove(userId);
        }
    }

    public void removeUserFromTopic(int genreId, int userId)
    {
        String genre = "";
        for (Integer gId:genreCodePerUser.keySet()) {
            if(gId == genreId)
            {
                for(Pair<Integer, String> pair : genreCodePerUser.get(genreId))
                {
                    if(pair.getKey() == userId)
                    {
                        genreCodePerUser.get(genreId).remove(pair);
                        genre = pair.getValue();
                    }
                }
            }
        }

        if(!genre.equals(""))
        {
            usersPerGenre.get(genre).remove(userId);
        }
    }

    public int getGenreId(String genreName, int userId)
    {
        for (Map.Entry<Integer, Queue<Pair<Integer, String>>> entry : genreCodePerUser.entrySet()) {
            for (Pair<Integer,String> pair: entry.getValue()) {
                if(pair.getValue().equals(genreName) & pair.getKey() == userId)
                {
                    return entry.getKey();
                }
            }
        }
        return -1;
    }
}
