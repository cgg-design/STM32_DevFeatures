pipeline {
    agent any

    parameters {
        booleanParam(
            name: 'FLASH_HARDWARE',
            defaultValue: true,
            description: 'Set to true to flash compiled firmware onto the STM32 board via ST-LINK'
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
    }

    stages {
        stage('Checkout') {
            steps {
                checkout scm
            }
        }

        stage('Bootstrap Tools (Doxygen & Cppcheck)') {
            steps {
                powershell '''
                    # 1. Install/Verify Doxygen Portable
                    $doxygenBin = "C:\\tools\\doxygen\\bin"
                    if (!(Test-Path "$doxygenBin\\doxygen.exe")) {
                        Write-Host "Doxygen missing. Downloading portable version to C:\\tools\\doxygen..."
                        New-Item -ItemType Directory -Force -Path "C:\\tools\\doxygen" | Out-Null
                        $url = "https://www.doxygen.nl/files/doxygen-1.11.0.windows.x64.bin.zip"
                        $zipPath = "$env:TEMP\\doxygen.zip"
                        Invoke-WebRequest -Uri $url -OutFile $zipPath
                        Expand-Archive -Path $zipPath -DestinationPath "C:\\tools\\doxygen" -Force
                        Remove-Item $zipPath
                        Write-Host "Doxygen successfully installed."
                    } else {
                        Write-Host "Doxygen is ready at C:\\tools\\doxygen."
                    }

                    # 2. Install/Verify Cppcheck via Chocolatey
                    if (!(Get-Command "cppcheck" -ErrorAction SilentlyContinue) -and !(Test-Path "C:\\Program Files\\Cppcheck\\cppcheck.exe")) {
                        if (Get-Command "choco" -ErrorAction SilentlyContinue) {
                            Write-Host "Cppcheck missing. Installing via Chocolatey..."
                            choco install cppcheck -y
                        } else {
                            Write-Warning "Cppcheck not found and Chocolatey is not installed. Please install Cppcheck or Chocolatey manually."
                        }
                    } else {
                        Write-Host "Cppcheck is ready."
                    }
                '''
            }
        }

        stage('Check Tools') {
            steps {
                bat '''
                    git --version
                    "%PIO%" --version
                    
                    :: Add Doxygen and Cppcheck paths to current session
                    set "PATH=C:\\tools\\doxygen\\bin;C:\\Program Files\\Cppcheck;%PATH%"
                    
                    cppcheck --version || echo Cppcheck not found in PATH
                    doxygen --version || echo Doxygen not found in PATH
                '''
            }
        }

        stage('Static Code Analysis') {
            steps {
                echo 'Running static code analysis with Cppcheck...'
                bat '''
                    set "PATH=C:\\Program Files\\Cppcheck;%PATH%"
                    cppcheck --version >nul 2>&1 && (
                        cppcheck --enable=all --inconclusive --xml --xml-version=2 src/ include/ 2> cppcheck-report.xml
                    ) || (
                        echo Cppcheck not found, skipping static analysis.
                    )
                '''
            }
        }

        stage('Generate Documentation') {
            steps {
                echo 'Generating Doxygen documentation...'
                bat '''
                    set "PATH=C:\\tools\\doxygen\\bin;%PATH%"
                    doxygen --version >nul 2>&1 && (
                        doxygen Doxyfile
                    ) || (
                        echo Doxygen execution failed or Doxyfile missing, skipping documentation generation.
                    )
                '''
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

    // post {
    //     success {
    //         echo 'STM32 pipeline successfully built, tested, and flashed!'
    //         emailext (
    //             subject: "SUCCESS: Job '${env.JOB_NAME} [Build #${env.BUILD_NUMBER}]'",
    //             body: "The pipeline completed successfully.\n\nCheck console output at: ${env.BUILD_URL}",
    //             to: 'c.guetsop@gmail.com'
    //         )
    //     }
    //     failure {
    //         echo 'Pipeline failed. Check the Jenkins console output.'
    //         emailext (
    //             subject: "FAILED: Job '${env.JOB_NAME} [Build #${env.BUILD_NUMBER}]'",
    //             body: "The pipeline failed during execution.\n\nCheck console output at: ${env.BUILD_URL}",
    //             to: 'c.guetsop@gmail.com'
    //         )
    //     }
    // }
}