package bgu.spl.net.srv;

public class Book {
    private String bookName;
    private String genre;

    public Book(String bookName, String genre)
    {
        this.bookName = bookName;
        this.genre = genre;
    }

    public String getBookName() {return this.bookName;}
    public void setBookName(String bookName) {this.bookName = bookName;}
    public String getBookGenre() {return this.genre;}
    public void setGenre(String genre) {this.genre = genre;}

}
