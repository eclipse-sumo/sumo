
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for Polygon_setType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="Polygon_setType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="polygonID" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="polygonType" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "Polygon_setType", propOrder = {
    "polygonID",
    "polygonType"
})
public class PolygonSetType {

    protected String polygonID;
    protected String polygonType;

    /**
     * Gets the value of the polygonID property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getPolygonID() {
        return polygonID;
    }

    /**
     * Sets the value of the polygonID property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setPolygonID(String value) {
        this.polygonID = value;
    }

    /**
     * Gets the value of the polygonType property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getPolygonType() {
        return polygonType;
    }

    /**
     * Sets the value of the polygonType property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setPolygonType(String value) {
        this.polygonType = value;
    }

}
