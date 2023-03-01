using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class test : MonoBehaviour
{


    public GameObject prefab;
    public float offsetZ;
    void Start()
    {
        //asdasdasdasdasdasd
        //GameObject.CreatePrimitive(PrimitiveType.)|
        for (int i = 0; i < 10; i++)
        {

            GameObject obj = Instantiate(prefab);
            offsetZ += 20;
            obj.transform.position = new Vector3(0, 0, offsetZ);

            Destroy(obj);
        }

    }
}
