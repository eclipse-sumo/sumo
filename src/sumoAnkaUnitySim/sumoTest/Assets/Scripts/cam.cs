using UnityEngine;

public class cam : MonoBehaviour
{
    public Transform vehicle;
    public float omega = 180;
    public float phi = 45;
    public float radius = 50;
    public float fov = 30;
    public float ratio = 0.5f;
    public bool freeCAM = false;
    float height;

    System.Random rand = new System.Random();
    private static float movementSpeed = 1.0f;

    private void Start()
    {
        height =   radius * Mathf.Sin(phi);
        
    }
 
    private void Update()
    {
         if (GameObject.Find("Vehicles").transform.childCount > 0)
        {
            if (Input.GetKeyDown(KeyCode.C) || vehicle == null)
            {
                vehicle = GameObject.Find("Vehicles").transform.GetChild(rand.Next(GameObject.Find("Vehicles").transform.childCount));
            }
            else if (Input.GetKeyDown(KeyCode.V))
            {
                freeCAM = !freeCAM;
            }
            fov += Input.GetAxis("Mouse ScrollWheel") * 10;
            Camera.main.fieldOfView = fov; 
        }
        if (vehicle != null && freeCAM)
        {
            
            

            float currentyaw = transform.eulerAngles.y;
            float currentheight = transform.position.y;

            //yaw = Mathf.LerpAngle(currentyaw, yaw, ratio * Time.deltaTime);
            //height = Mathf.LerpAngle(currentheight, height, ratio * Time.deltaTime);
            omega += Input.GetAxis("Mouse X") * 3;
            height -= Input.GetAxis("Mouse Y") * 3;

            Quaternion currentRotation = Quaternion.Euler(0, omega, 0);
            transform.position = vehicle.position;
            transform.position -= currentRotation * Vector3.forward * radius;

            Vector3 temp = transform.position;
            temp.y = height;
            transform.position = temp;

            transform.LookAt(vehicle);

        }
        else
        {
            movementSpeed = Mathf.Max(movementSpeed += Input.GetAxis("Mouse ScrollWheel"), 0.0f);
            transform.position += (transform.right * Input.GetAxis("Horizontal") + transform.forward * Input.GetAxis("Vertical")  ) * movementSpeed;
            transform.eulerAngles += new Vector3(-Input.GetAxis("Mouse Y")*5, Input.GetAxis("Mouse X")*5, 0);
        }
    }
}