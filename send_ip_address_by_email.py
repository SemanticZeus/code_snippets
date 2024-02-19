#!/usr/bin/env python
import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
import socket
import configparser
import datetime

config = configparser.ConfigParser()
config.read('/usr/bin/email.conf')

def get_ip_address():
    try:
        # Create a socket object
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        # Connect to a remote server (doesn't matter which server)
        s.connect(("8.8.8.8", 80))

        # Get the local IP address
        ip_address = s.getsockname()[0]

        return ip_address
    except socket.error as e:
        print(f"Error: {e}")
    finally:
        s.close()

# Yahoo Mail SMTP server details
smtp_server = 'smtp.mail.yahoo.com'
smtp_port = 465  # SSL port

sender_email = config.get('Email', 'sender_email')
password = config.get('Email', 'app_pass')

# Recipient email address
recipient_email = config.get('Email', 'recipient_email')

# Create a multipart message and set headers
message = MIMEMultipart()
message['From'] = sender_email
message['To'] = recipient_email
message['Subject'] = 'raspberry pi IP address'

def log(message):
    message = message + '   {:%Y-%m-%d %H:%M:%S}\n'.format(datetime.datetime.now())
    with open('/home/pi/email.log', 'a') as f:
        f.write(message)

# Add body to email
for i in range(20):
    try:
        body = 'IP adress = ' + get_ip_address() + '\nHost Name = ' + socket.gethostname()
        message.attach(MIMEText(body, 'plain'))
    except Exception as e:
        log(f'Failed to get ip address try number {i}. {str(e)}')
        continue

    # Create SMTP session for sending the mail
    try:
        server = smtplib.SMTP_SSL(smtp_server, smtp_port)
        server.login(sender_email, password)
        server.sendmail(sender_email, recipient_email, message.as_string())
        log('Email sent successfully!')
    except Exception as e:
        log(f'Failed to send email. Error: {str(e)}')
        continue
    finally:
        server.quit()
    break
