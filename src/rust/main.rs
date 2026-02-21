use std::io::prelude::*;
use std::os::unix::net::UnixStream;
use std::fs::OpenOptions;
use std::io::Write;
use std::time::SystemTime;

fn writeLog(filename: &str, message: &str) {
    if let Ok(mut file) = OpenOptions::new().create(true).append(true).open(filename) {
        let now = SystemTime::now();
        let datetime: chrono::DateTime<chrono::Local> = now.into();
        let timestamp = datetime.format("%Y-%m-%d %H:%M:%S").to_string();
        println!("[{}] {}", timestamp, message);
        let _ = writeln!(file, "[{}] {}", timestamp, message);
    }
}

fn main() {
    let socketPath = "/tmp/project6_socket";
    let logFile = "rust_client.log";
    let myId = "RustClient";

    writeLog(logFile, "Клиент запускается. Подключение к серверу...");

    match UnixStream::connect(socketPath) {
        Ok(mut stream) => {
            writeLog(logFile, "Подключено к серверу");

            if let Err(e) = stream.write_all(myId.as_bytes()) {
                writeLog(logFile, &format!("Не удалось отправить ID: {}", e));
                return;
            }
            writeLog(logFile, &format!("Отправлено: {}", myId));

            let mut response = String::new();
            if let Err(e) = stream.read_to_string(&mut response) {
                writeLog(logFile, &format!("Не удалось прочитать ответ: {}", e));
                return;
            }
            writeLog(logFile, &format!("Получено от сервера: {}", response.trim()));
        }
        Err(e) => {
            writeLog(logFile, &format!("Не удалось подключиться: {}", e));
        }
    }

    writeLog(logFile, "Клиент завершил работу");
}
