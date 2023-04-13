from selenium import webdriver
from webdriver_manager.microsoft import EdgeChromiumDriverManager
from selenium.webdriver.edge.service import Service as EdgeService
from selenium.webdriver.common.by import By
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.wait import WebDriverWait

from selenium.webdriver.edge.options import Options
options = Options()
options.add_argument('--headless')
options.add_argument('--window-size=1920,1080')
options.add_argument('--disable-gpu')


driver = webdriver.Edge(service = EdgeService(EdgeChromiumDriverManager().install()), options = options)

driver.get("https://docs.python.org/3/tutorial/index.html")
driver.maximize_window()

driver.close()