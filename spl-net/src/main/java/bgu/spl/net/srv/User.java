package bgu.spl.net.srv;

public class User {
    private String userName;
    private String password;

    public User(String userName, String password)
    {
        this.password = password;
        this.userName = userName;
    }

    public String getUserName() {return this.userName;}
    public void setUserName(String userName) {this.userName = userName;}
    public String getPassword() {return this.password;}
    public void setPassword(String password) {this.password = password;}

}
