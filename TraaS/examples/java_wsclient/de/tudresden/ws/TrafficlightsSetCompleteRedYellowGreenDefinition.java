
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java-Klasse für Trafficlights_setCompleteRedYellowGreenDefinition complex type.
 * 
 * <p>Das folgende Schemafragment gibt den erwarteten Content an, der in dieser Klasse enthalten ist.
 * 
 * <pre>
 * &lt;complexType name="Trafficlights_setCompleteRedYellowGreenDefinition">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="tlsID" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="tls" type="{http://ws.tudresden.de/}sumoTLSProgram" minOccurs="0"/>
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
    protected SumoTLSProgram tls;

    /**
     * Ruft den Wert der tlsID-Eigenschaft ab.
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
     * Legt den Wert der tlsID-Eigenschaft fest.
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
     * Ruft den Wert der tls-Eigenschaft ab.
     * 
     * @return
     *     possible object is
     *     {@link SumoTLSProgram }
     *     
     */
    public SumoTLSProgram getTls() {
        return tls;
    }

    /**
     * Legt den Wert der tls-Eigenschaft fest.
     * 
     * @param value
     *     allowed object is
     *     {@link SumoTLSProgram }
     *     
     */
    public void setTls(SumoTLSProgram value) {
        this.tls = value;
    }

}
