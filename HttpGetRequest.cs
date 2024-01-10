using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.UI;
using System.Collections;

public class HttpGetRequest : MonoBehaviour
{
    public Text sensorDataText;
    public string arduinoIPAddress = "your-arduino-ip-address";
    public float refreshInterval = 1f; // Interval v sekundah za osveževanje podatkov

    void Start()
    {
        StartCoroutine(GetSensorData());
    }

    IEnumerator GetSensorData()
    {
        while (true)
        {
            UnityWebRequest www = UnityWebRequest.Get("http://" + arduinoIPAddress);
            yield return www.SendWebRequest();

            if (www.isNetworkError || www.isHttpError)
            {
                Debug.Log(www.error);
            }
            else
            {
                sensorDataText.text = www.downloadHandler.text;
            }

            yield return new WaitForSeconds(refreshInterval);
        }
    }
}
