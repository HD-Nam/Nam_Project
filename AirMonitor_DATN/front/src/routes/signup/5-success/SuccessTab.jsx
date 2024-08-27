import { Text, MarkupText } from 'preact-i18n';
import { Link } from 'preact-router/match';

const ConfigureHouseTab = () => (
  <div class="card">
    <div class="card-body">
      <div class="row">
        <div class="col-lg-6">
          <div class="d-flex flex-column p-2">
            <div>
              <h2>
                <Text id="signup.success.title" />
              </h2>
              <p class="text">
                <MarkupText id="signup.success.successfullySetup" />
              </p>
            </div>

            <div>
              <Link class="btn btn-primary" href="/dashboard">
                <Text id="signup.success.goToDashboardButton" />
              </Link>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
);

export default ConfigureHouseTab;
