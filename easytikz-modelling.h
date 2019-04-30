
class Shape
{
private:
    /* data */
public:
    Shape(/* args */);
    ~Shape();
};

Shape::Shape(/* args */)
{
}

Shape::~Shape()
{
}

class Connection
{
private:
    /* data */
public:
    Connection
(/* args */);
    ~Connection
();
};

Connection::Connection(/* args */)
{
}

Connection::~Connection()
{
}



class Diagram
{
private:
    /* data */
public:
    Diagram(/* args */);
    ~Diagram();
    void insertShape();
    void insertConnection();
    Shape getShape();
    Connection getConnection();
};

Diagram::Diagram(/* args */)
{
}

Diagram::~Diagram()
{
}
