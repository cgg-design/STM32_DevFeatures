pipeline {
    agent any

    parameters {
        booleanParam(
            name: 'FLASH_HARDWARE',
            defaultValue: true,
            description: 'Set to true to flash the compiled firmware onto the STM32 board via ST-LINK'
        )
        booleanParam(
            name: 'RUN_TESTS',
            defaultValue: true,
            description: 'Execute unit tests before flashing'
        )
    }

    options {
        timestamps()
        disableConcurrentBuilds()
    }

    environment {
        PIO = 'C:\\tools\\pio-venvr\\Scripts\\pio.exe'
        PIO_ENV = 'nucleo_f446re'
        DOXYGEN = 'C:\\Program Files\\doxygen\\bin\\doxygen.exe' // Path to Doxygen executable
        CPPCHECK = 'C:\\Program Files\\Cppcheck\\cppcheck.exe'  // Path to Cppcheck executable
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

        stage('Static Code Analysis') {
            steps {
                echo 'Running static code analysis with Cppcheck...'
                // Generates an XML report of code defects and warnings
                bat '''
                    if exist "%CPPCHECK%" (
                        "%CPPCHECK%" --enable=all --inconclusive --xml --xml-version=2 src/ include/ 2> cppcheck-report.xml
                    ) else (
                        echo Cppcheck not installed, skipping static analysis.
                    )
                '''
            }
        }

        stage('Generate Documentation') {
            steps {
                echo 'Generating Doxygen documentation...'
                bat '''
                    if exist "%DOXYGEN%" (
                        "%DOXYGEN%" Doxyfile
                    ) else (
                        echo Doxygen not found, skipping documentation generation.
                    )
                '''
                // Publish Doxygen HTML documentation on the Jenkins UI
                publishHTML([
                    allowMissing: true,
                    alwaysLinkToLastBuild: true,
                    keepAll: true,
                    reportDir: 'docs/html',
                    reportFiles: 'index.html',
                    reportName: 'Doxygen API Documentation'
                ])
            }
        }

        stage('Build Firmware') {
            steps {
                bat '"%PIO%" run -e %PIO_ENV%'
            }
        }

        stage('Run Unit Tests') {
            when {
                expression {
                    return "${params.RUN_TESTS}".toBoolean() == true
                }
            }
            steps {
                echo 'Executing PlatformIO Unit Tests...'
                // Runs native or target unit tests configured in platformio.ini
                bat '"%PIO%" test -e %PIO_ENV%'
            }
        }

        stage('Flash STM32') {
            when {
                expression {
                    return "${params.FLASH_HARDWARE}".toBoolean() == true
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

        stage('Archive Artifacts & Reports') {
            steps {
                archiveArtifacts(
                    artifacts: '.pio/build/nucleo_f446re/firmware.elf, .pio/build/nucleo_f446re/firmware.bin, cppcheck-report.xml',
                    allowEmptyArchive: true,
                    fingerprint: true
                )
            }
        }
    }

    post {
        success {
            echo 'STM32 firmware build, testing, and documentation completed successfully!'
        }
        failure {
            echo 'Pipeline failed. Check the Jenkins console output.'
        }
    }
}