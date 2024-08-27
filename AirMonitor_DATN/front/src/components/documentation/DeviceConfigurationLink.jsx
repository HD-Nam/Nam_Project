import cx from 'classnames';

const DeviceConfigurationLink = ({ children, configurationKey, documentKey, user, linkClass }) => (
  <a
    target="_blank"
    rel="noopener noreferrer"
    href={'/404'}
    class={cx({
      [linkClass]: linkClass
    })}
  >
    {children}
  </a>
);

export default DeviceConfigurationLink;
