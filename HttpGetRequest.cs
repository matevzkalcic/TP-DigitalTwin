using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.UI;
using System.Collections;

public class HttpGetRequest : MonoBehaviour
{
    public Text sensorDataText;
    public string arduinoIPAddress = "your-arduino-ip-address";

    void Start()
    {
        StartCoroutine(GetSensorData());
    }

    IEnumerator GetSensorData()
    {
        UnityWebRequest www = UnityWebRequest.Get("http://" + arduinoIPAddress);
        yield return www.SendWebRequest();

        if (www.isNetworkError || www.isHttpError)
        {
            Debug.Log(www.error);
        }
        else
        {
            // Show the sensor data in the Unity UI Text component
            sensorDataText.text = www.downloadHandler.text;
        }
    }
}
