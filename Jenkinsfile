
pipeline {
    agent any

    options {
        timestamps()
        disableConcurrentBuilds()
    }

    environment {
        PIO = 'C:\\tools\\pio-venvr\\Scripts\\pio.exe'
        PIO_ENV = 'nucleo_f446re'
    }

    stages {
        stage('Checkout') {
            steps {
                checkout scm
            }
        }

        stage('Check Tools') {
            steps {
                bat '''
                    git --version
                    "%PIO%" --version
                '''
            }
        }

        stage('Build Firmware') {
            steps {
                bat '''
                    "%PIO%" run -e %PIO_ENV%
                '''
            }
        }
        stage('Flash STM32') {
            when {
                expression {
                    params.FLASH_HARDWARE == true
                }
            }
            steps {
                bat '"%PIO%" run -e %PIO_ENV% -t upload'
            }
        }

        stage('Verify Artifacts') {
            steps {
                bat '''
                    if not exist ".pio\\build\\%PIO_ENV%\\firmware.elf" exit /b 1
                    if not exist ".pio\\build\\%PIO_ENV%\\firmware.bin" exit /b 1
                '''
            }
        }

        stage('Archive Firmware') {
            steps {
                archiveArtifacts(
                    artifacts: '.pio/build/nucleo_f446re/firmware.elf,.pio/build/nucleo_f446re/firmware.bin',
                    fingerprint: true
                )
            }
        }
    }

    post {
        success {
            echo 'STM32 firmware build succeeded.'
        }

        failure {
            echo 'Pipeline failed. Check the Jenkins console output.'
        }
    }
}
