package bgu.spl.net.srv;

import javafx.util.Pair;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class UsersHandler {

    private Map<Integer, User> mapOfActiveUsers;
    private Map<User,Integer> mapOfUsers;

    private static class SingletonHolder {
        private static UsersHandler instance = new UsersHandler();
    }

    public static UsersHandler getInstance() {
        return UsersHandler.SingletonHolder.instance;
    }

    private UsersHandler()
    {
        mapOfActiveUsers = new ConcurrentHashMap<>();
        mapOfUsers= new ConcurrentHashMap<>();
    }



    //for the login frame
    public String addNewUser(String userName, String password, int userId)
    {
        //the check of the socket will happen outside of the function
        User currUser = null;
        boolean found = false;
        for (User user :mapOfUsers.keySet()) {
            if (!found && user.getUserName().equals(userName)) {
                found = true;
                currUser = user;
                break;
            }
        }
        if(found) //the user is in the system
        {
            /*when the user connected once we created an id. when he disconnects and then connects again,
             *the ConnectionHandler will create a new id to the client,
             *and because the client already exists in the system it has an id, so we will update his id to the new id*/
            int oldId = mapOfUsers.get(currUser);
            if(!currUser.getPassword().equals(password)) //the password is incorrect
            {
                return "Wrong password";
            }
            if(mapOfActiveUsers.containsKey(oldId)) // the password is correct but the user is already logged in
            {
                return "User already logged in";
            }
            mapOfActiveUsers.put(userId,currUser);
            if(mapOfUsers.get(currUser) != userId)
            {
                mapOfUsers.remove(currUser,oldId);
                mapOfUsers.put(currUser,userId);
            }
            return "success"; //login successful
        }
        //add new user to the system
        User u  = new User(userName,password);
        mapOfUsers.put(u, userId);
        mapOfActiveUsers.put(userId, u);
        return "success";//login successful
    }

    public boolean checkIfUserIsActive(int userId) {
        return mapOfActiveUsers.containsKey(userId);
    }

    public void removeActiveUser(int userId)
    {
        this.mapOfActiveUsers.remove(userId);
    }
}
