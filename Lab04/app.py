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

driver.get("https://www.nycu.edu.tw")
driver.maximize_window()
news = WebDriverWait(driver, 10).until(EC.element_to_be_clickable((By.LINK_TEXT, "新聞")))
# news = driver.find_element(By.LINK_TEXT, "新聞")
news.click()

first_news = WebDriverWait(driver, 10).until(EC.element_to_be_clickable((By.XPATH, "//li[@class = 'su-post'][1]/a")))
first_news.click()

title = WebDriverWait(driver, 10).until(EC.element_to_be_clickable((By.TAG_NAME, "h1")))
print(title.text)

content = WebDriverWait(driver, 10).until(EC.element_to_be_clickable((By.XPATH, "//div[@class = 'entry-content clr']")))
print(content.text)

driver.switch_to.new_window("tab")
driver.get("https://www.google.com/")

input = WebDriverWait(driver, 10).until(EC.element_to_be_clickable((By.XPATH, "//input[@class = 'gLFyf']")))
input.send_keys("311551152")

submit = WebDriverWait(driver, 10).until(EC.element_to_be_clickable((By.XPATH, "//input[@type = 'submit']")))
submit.click()

second_title = WebDriverWait(driver, 10).until(EC.element_to_be_clickable((By.XPATH, "//div[@class = 'MjjYud'][2]//h3")))
print(second_title.text)

driver.close()