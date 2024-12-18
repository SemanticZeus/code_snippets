import requests

# URL of the server
url = 'https://www.jamesaminian.com/evomemora/'
#url = 'https://10.0.0.208/evomemora/'

# Data to be sent in the POST request
payload = {
    'username': 'heather',
    'password': 'asderf'
}

file_path = 'list.txt'

try:
    # Sending the POST request
    with open(file_path, 'rb') as file:
        files = {'file': (file.name, file, 'text/plain')}
        response = requests.post(url, data=payload, files=files)
        response.raise_for_status()
        print(response.request.url)
        print(response.request.body)
        print(response.request.headers)
    
    # Print the response from the server
    print(f"Status Code: {response.status_code}")
    print("Response Text:", response.text)
    
    # If the response is in JSON format
    try:
        response_json = response.json()
        print("JSON Response:", response_json)
    except ValueError:
        print("Response is not in JSON format")

except requests.exceptions.HTTPError as http_err:
    print(f"HTTP error occurred: {http_err}")
except Exception as err:
    print(f"Other error occurred: {err}")
