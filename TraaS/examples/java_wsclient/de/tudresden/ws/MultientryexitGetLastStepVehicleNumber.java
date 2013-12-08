
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for Multientryexit_getLastStepVehicleNumber complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="Multientryexit_getLastStepVehicleNumber">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="detID" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "Multientryexit_getLastStepVehicleNumber", propOrder = {
    "detID"
})
public class MultientryexitGetLastStepVehicleNumber {

    protected String detID;

    /**
     * Gets the value of the detID property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getDetID() {
        return detID;
    }

    /**
     * Sets the value of the detID property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setDetID(String value) {
        this.detID = value;
    }

}
