import scrapy
import os
from os import path
import time
import requests
import shutil


class LozaSpider(scrapy.Spider):
    name = "crawl_products"
    start_urls = [
        "https://tiki.vn/ao-so-mi-nam-tay-dai/c10379",
        "https://tiki.vn/ao-so-mi-nam-tay-ngan/c10380",
        "https://tiki.vn/ao-khoac-kieu-nam/c10381",
        "https://tiki.vn/ao-thun-nam/c917",
        "https://tiki.vn/quan-jeans-nam/c920",
        "https://tiki.vn/thoi-trang-nam-quan-sooc/c923",
        "https://tiki.vn/ao-dai/c10385",
        "https://tiki.vn/chan-vay-nu/c5404",
        "https://tiki.vn/dam-vay-lien/c941",

    ]
    def parse(self, response):
        category = response.url.split('/')[3]
        base_url = "https://tiki.vn"
        url_next_page = response.xpath('//a[@class="next"]/@href').get()
        url_next_page = base_url+url_next_page
        name_category = response.xpath(
            '/html/body/div[10]/div/div[2]/div[1]/div/h1/text()').get().replace('\n', '').replace(':', '').strip()
        #page number
        if url_next_page.find('page=10') == -1:
        #     print("----------------------",url_next_page)
            for product_link in response.xpath('/html/body/div[10]/div/div/div/div/div/a/@href').getall():
                yield scrapy.Request(product_link, callback=self.parse_info, cb_kwargs=dict(category=category, name_category=name_category))
            yield scrapy.Request(url_next_page, callback=self.parse)

    def parse_info(self, response, category, name_category):
        link_item = response.url
        path_category = 'image/' + category
        path_file = 'image/' + category+'/' + \
            '-'.join(link_item.split('/')[3].split('?')[0].split('-')[:-1])
        name = response.xpath('//*[@id="product-name"]/span[3]/text()').get()
        price = response.xpath('//*[@id="span-price"]/text()').get()
        brand = response.xpath(
            '/html/body/div[10]/div[2]/div[1]/div[2]/div/div[1]/div/div/div[3]/div[2]/p/a/text()').get()
        content = response.xpath(
            '/html/body/div[10]/div[2]/div[1]/div[2]/div/div[2]/div[1]/div/div/div[2]/p/text()').getall()
        if len(content) >= 2:
            description = 'space'.join(content)
        else:
            description = content
        # print(">>>>>>>>>>>>>>>>>>>>>>",response.xpath('//img[@alt="Product"]/@src').getall())
        dictionary = {
            "category": name_category,
            "name": name,
            "price": price,
            "brand": brand,
            "description": description,
            "link_item_tiki": link_item,
            "path_image_file": path_file,
            }
        if len(response.xpath('//img[@alt="Product"]/@src').getall()) > 0:
            if path.isdir('image') == False:
                os.mkdir('image')
            if path.isdir(path_category) == False:
                os.mkdir(path_category)  
            if path.isdir(path_file) == False:
                os.mkdir(path_file)
            for image_url in response.xpath('//img[@alt="Product"]/@src').getall():
                print("++++++++++++++++++++++++++++++++++++++++++++", image_url)
            #     path_image_file = path_file + '/' + image_url.split('/')[-1]
            #     resp = requests.get(image_url, stream=True)
            #     resp = requests.get(image_url, stream=True)
            #     local_file = open(path_image_file, 'wb')
            #     resp.raw.decode_content = True
            #     shutil.copyfileobj(resp.raw, local_file)
            #     del resp
            #     local_file.close()
                yield scrapy.Request(image_url, callback=self.parse_image, cb_kwargs=dict(dictionary = dictionary))
        # yield  scrapy.Request(image, callback=self.parse_image)
    def parse_image(self, response, dictionary):
        path_image_file = dictionary['path_image_file'] +'/'+ response.url.split('/')[-1]
        with open(path_image_file, 'wb+') as f:
            f.write(response.body)
        yield {
            "category": dictionary['category'],
            "name":dictionary['name'],
            "price": dictionary['price'],
            "brand": dictionary['brand'],
            "description": dictionary['description'],
            "link_item_tiki": dictionary['link_item_tiki'],
            "path_image_file": path_image_file,
            "image_url": response.url
            }
