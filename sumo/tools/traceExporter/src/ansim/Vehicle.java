package ansim;

/**
* model for vehicles
* @author Thimor Bohn <bohn@itm.uni-luebeck.de>
*/
public class Vehicle {
   
   /**
    * constructor
    * @param id
    * @param x
    * @param y
    */
   public Vehicle(String id, float x, float y) {
       this.id = id;
       this.x = x;
       this.y = y;
   }
   
   public String id;
   public float x;
   public float y;
}
