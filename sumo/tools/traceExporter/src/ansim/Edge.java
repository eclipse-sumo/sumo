package ansim;

/**
* model for edges
* @author Thimor Bohn <bohn@itm.uni-luebeck.de>
*/
public class Edge {
   
   /**
    * constructor
    * @param id
    * @param length
    * @param speed
    * @param name
    * @param xfrom
    * @param yfrom
    * @param xto
    * @param yto
    */
   public Edge(String id, float length, float speed, String name, float xfrom, float yfrom, float xto, float yto) {
       this.id = id;
       this.length = length;
       this.speed = speed;
       this.name = name;
       this.xfrom = xfrom;
       this.yfrom = yfrom;
       this.xto = xto;
       this.yto = yto;
   }
       
   public String id;
   public float length;
   public float speed;
   public String name;
   public float xfrom;
   public float yfrom;
   public float xto;
   public float yto;
}
