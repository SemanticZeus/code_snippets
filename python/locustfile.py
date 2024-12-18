from locust import HttpUser, task, between

class WebsiteTest(HttpUser):
    # Users wait between 5 and 15 seconds before repeating their task
    wait_time = between(5, 15)
    
    @task
    def load_homepage(self):
        # GET request to the homepage
        self.client.get("/")

# The script is set to spawn 1000 total users with a hatch rate (users spawned per second) of 10
# To run this script, save it as locustfile.py and run `locust` from the terminal in the same directory

