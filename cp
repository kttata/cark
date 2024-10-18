$logfile = "$psscriptroot\RemoveUserFromGroup_logs.txt"
Write-host "==========================================================="
Write-host "Start of script [$(get-date -Format "dd/MM/yyyy hh:mm:ss")]"
write-host " "
Start-Transcript -Path $logfile -Append -Force
 
Import-Module 'C:\psPAS\psPAS\pspas.psm1'
 
$PVWAURL =https://company.com/
 
$safe=""
$folder="root"
$object=""
$appid=""
$reason= "testing"
$clisdkpath="C:\Program Files (x86)\CyberArk\ApplicationPasswordSdk\CLIPasswordSDK.exe"
$query = "safe=$safe;Folder=$folder;Object=$object"
$group ="psmappusers"
 
$password = .$clisdkpath GetPassword /p AppDescs.AppID=$appid /p Query=$query /p Reason=$reason  /o Password
$username = .$clisdkpath GetPassword /p AppDescs.AppID=$appid /p Query=$query /p Reason=$reason /o  PassProps.UserName
$Password = $password | ConvertTo-SecureString -AsPlainText -Force
$cred = New-Object -TypeName System.Management.Automation.PSCredential -ArgumentList $Username, $Password
 
if($cred)
{
 
 
        New-PASSession -Credential $cred -BaseURI $PVWAURL -type CyberArk -concurrentSession 20 -ErrorAction SilentlyContinue -ErrorVariable LogonErr
        $Loggeduser = Get-PASSession -ErrorAction SilentlyContinue -ErrorVariable GetUsrERR
        if($Loggeduser.user -eq $Username)
        {
 
          $GroupDetails = Get-PASGroup -search $group -includeMembers $true
          $members = $GroupDetails.members.username
          $groupID=$GroupDetails.id
          if($members)
            {
                $provusers=$members -like "prov_*"
                if($provusers)
                    {
                      foreach($user in $provusers)
                        {
 
                          Write-host "Removing $user from group $group"
                 
                          Remove-PASGroupMember -GroupID $groupID -Member $user
 
                        }
                    }
            }
 
            Disconnect-PSSession
 
        }
        else
        {
         Write-host "failed to login to vault, error $LogonErr"
         }
 
}
 
Write-host "==========================================================="
Write-host "End of script [$(get-date -Format "dd/MM/yyyy hh:mm:ss")]"
write-host " "
Stop-Transcript
