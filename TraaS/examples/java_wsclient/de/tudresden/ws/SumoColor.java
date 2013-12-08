
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for sumoColor complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="sumoColor">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="r" type="{http://www.w3.org/2001/XMLSchema}byte"/>
 *         &lt;element name="g" type="{http://www.w3.org/2001/XMLSchema}byte"/>
 *         &lt;element name="b" type="{http://www.w3.org/2001/XMLSchema}byte"/>
 *         &lt;element name="a" type="{http://www.w3.org/2001/XMLSchema}byte"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "sumoColor", propOrder = {
    "r",
    "g",
    "b",
    "a"
})
public class SumoColor {

    protected byte r;
    protected byte g;
    protected byte b;
    protected byte a;

    /**
     * Gets the value of the r property.
     * 
     */
    public byte getR() {
        return r;
    }

    /**
     * Sets the value of the r property.
     * 
     */
    public void setR(byte value) {
        this.r = value;
    }

    /**
     * Gets the value of the g property.
     * 
     */
    public byte getG() {
        return g;
    }

    /**
     * Sets the value of the g property.
     * 
     */
    public void setG(byte value) {
        this.g = value;
    }

    /**
     * Gets the value of the b property.
     * 
     */
    public byte getB() {
        return b;
    }

    /**
     * Sets the value of the b property.
     * 
     */
    public void setB(byte value) {
        this.b = value;
    }

    /**
     * Gets the value of the a property.
     * 
     */
    public byte getA() {
        return a;
    }

    /**
     * Sets the value of the a property.
     * 
     */
    public void setA(byte value) {
        this.a = value;
    }

}
