
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for Inductionloop_getLastStepVehicleIDs complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="Inductionloop_getLastStepVehicleIDs">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="loopID" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "Inductionloop_getLastStepVehicleIDs", propOrder = {
    "loopID"
})
public class InductionloopGetLastStepVehicleIDs {

    protected String loopID;

    /**
     * Gets the value of the loopID property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getLoopID() {
        return loopID;
    }

    /**
     * Sets the value of the loopID property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setLoopID(String value) {
        this.loopID = value;
    }

}
