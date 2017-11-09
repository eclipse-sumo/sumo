
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java-Klasse für sumoColor complex type.
 * 
 * <p>Das folgende Schemafragment gibt den erwarteten Content an, der in dieser Klasse enthalten ist.
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
     * Ruft den Wert der r-Eigenschaft ab.
     * 
     */
    public byte getR() {
        return r;
    }

    /**
     * Legt den Wert der r-Eigenschaft fest.
     * 
     */
    public void setR(byte value) {
        this.r = value;
    }

    /**
     * Ruft den Wert der g-Eigenschaft ab.
     * 
     */
    public byte getG() {
        return g;
    }

    /**
     * Legt den Wert der g-Eigenschaft fest.
     * 
     */
    public void setG(byte value) {
        this.g = value;
    }

    /**
     * Ruft den Wert der b-Eigenschaft ab.
     * 
     */
    public byte getB() {
        return b;
    }

    /**
     * Legt den Wert der b-Eigenschaft fest.
     * 
     */
    public void setB(byte value) {
        this.b = value;
    }

    /**
     * Ruft den Wert der a-Eigenschaft ab.
     * 
     */
    public byte getA() {
        return a;
    }

    /**
     * Legt den Wert der a-Eigenschaft fest.
     * 
     */
    public void setA(byte value) {
        this.a = value;
    }

}
