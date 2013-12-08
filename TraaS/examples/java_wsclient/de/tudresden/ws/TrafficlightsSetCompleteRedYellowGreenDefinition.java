
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for Trafficlights_setCompleteRedYellowGreenDefinition complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="Trafficlights_setCompleteRedYellowGreenDefinition">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="tlsID" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="tls" type="{http://ws.tudresden.de/}sumoTLSLogic" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "Trafficlights_setCompleteRedYellowGreenDefinition", propOrder = {
    "tlsID",
    "tls"
})
public class TrafficlightsSetCompleteRedYellowGreenDefinition {

    protected String tlsID;
    protected SumoTLSLogic tls;

    /**
     * Gets the value of the tlsID property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getTlsID() {
        return tlsID;
    }

    /**
     * Sets the value of the tlsID property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setTlsID(String value) {
        this.tlsID = value;
    }

    /**
     * Gets the value of the tls property.
     * 
     * @return
     *     possible object is
     *     {@link SumoTLSLogic }
     *     
     */
    public SumoTLSLogic getTls() {
        return tls;
    }

    /**
     * Sets the value of the tls property.
     * 
     * @param value
     *     allowed object is
     *     {@link SumoTLSLogic }
     *     
     */
    public void setTls(SumoTLSLogic value) {
        this.tls = value;
    }

}
