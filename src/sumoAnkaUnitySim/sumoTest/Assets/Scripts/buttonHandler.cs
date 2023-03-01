using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class buttonHandler : MonoBehaviour
{
    Button controlButton;
    bool gameControl = true;
    
    void Start()
    {
        controlButton = GameObject.FindGameObjectWithTag("canvas").GetComponentInChildren<Button>();
        controlButton.onClick.AddListener(onClicked);
    }
 
    void onClicked()
    {
        if (gameControl)
        {
            Time.timeScale = 1;
            controlButton.transform.Find("Text").GetComponent<TMP_Text>().text = "DURDUR";
        }
        else
        {
            Time.timeScale = 0;
            controlButton.transform.Find("Text").GetComponent<TMP_Text>().text = "BASLAT";
        }
        gameControl = !gameControl;
    }
}
