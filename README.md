Bramp adalah sebuah produk untuk membantu librarian mendeteksi buku-buku yang diambil dan yang dikembalikan di rak. 
Kami menggunakan ESP32, Reed Switch, Tilt Switch, LoadCell, LCD, dan Firebase. 
Prinsip kerjanya adalah memadukan kemampuan dari ketiga sensor untuk mendeteksi buku yang dikembalikan. Firebase sebagai database utama dapat dimanfaatkan oleh admin/librarian untuk mendeteksi apakah buku sudah dikembalikan dengan tepat di rak yang tepat? 
Dari sisi peminjam buku, LCD akan menampilkan deskripsi apakah buku yang dikembalikan sudah tepat atau belum.
Loadcell akan mendeteksi berat buku yang dikembalikan pada rak, jika buku yang dikembalikan tepat adanya beban akan mengaktifkan reed switch dan tilt dan mengirimkan pesan ke database secara real time melalui ESP32. 
Kami juga menambahkan fungsi lainnya, misal buku yang dikembalikan tidak memenuhi syarat. 
