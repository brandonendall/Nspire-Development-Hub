from io import BytesIO
from PIL import Image
from reportlab.lib.utils import ImageReader
from reportlab.pdfgen.canvas import Canvas

pdf = Canvas("proof.pdf.tns", pagesize=(200, 200))
pdf.setFont("Helvetica", 18)
pdf.drawString(10, 100, "EndraPDF text test")
pdf.showPage()
jpeg = BytesIO()
Image.new("RGB", (100, 100), (240, 0, 0)).save(jpeg, "JPEG")
jpeg.seek(0)
pdf.drawImage(ImageReader(jpeg), 30, 30, width=140, height=140)
pdf.save()
