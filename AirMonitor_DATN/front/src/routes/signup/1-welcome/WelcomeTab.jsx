import style from '../style.css';
import { Text, MarkupText } from 'preact-i18n';
import { Link } from 'preact-router/match';

const WelcomeStep = () => (
  <div class="d-flex flex-column flex-lg-row">
    <div class="col-lg-6">
      <h2 class={style.signupTitle}>
        <Text id="signup.welcome.title" />
      </h2>
      <p>
        <Text id="signup.welcome.introSentence" />
      </p>
    </div>
    <div class="col-lg-6 d-flex flex-row align-items-center">
      <div class="w-100">
        <Link class="btn btn-primary btn-block" href="/signup/create-account-local">
          <i class="fe fe-mail mr-1" /> <Text id="signup.welcome.buttonCreateAccountWithEmail" />
        </Link>
      </div>
    </div>
  </div>
);

export default WelcomeStep;
