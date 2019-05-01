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

class Shape
{
private:
    float rootCoordX;
    float rootCoordY;
protected:
    
public:
    Shape(/* args */);
    ~Shape();
    float getRootCoordX();
    virtual string toTikZ() = 0;
    float getRootCoordX();
    float getRootCoordY();
    void setRootCoordX(float input);
    void setRootCoordY(float input);
};

class Connection
{
private:
    /* data */
public:
    Connection(/* args */);
    ~Connection();
    

};
//=======================================================
//==== Shape derivatives ================================
//=======================================================
//=======================================================

class Rectangle:Shape
{
	private:

	protected:
		int verts;
		float size;
		//group of float[2][1] arrays for connection hardpoints?

	public:
		float getSize();
		int getVerts();
		void setSize(float input);
		void setVerts(int input);
		Rectangle();

		Rectangle(float rootCoordX, float rootCoordY);
		
};